/**
    @title     MoonBase
    @file      FilesService.cpp
    @repo      https://github.com/ewowi/MoonBase, submit changes to this file as PRs
    @Authors   https://github.com/ewowi/MoonBase/commits/main
    @Doc       https://ewowi.github.io/MoonBase/system/files/
    @Copyright © 2025 Github MoonBase Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#if FT_MOONBASE == 1

#include "FilesService.h"

#include <ESPFS.h>

void walkThroughFiles(File folder, std::function<void(File, File)> fun) {
	folder.rewindDirectory();
	while (true)
	{
		File file = folder.openNextFile();
        if (!file) break;

        fun(folder, file);
        
        if (file.isDirectory()) {
            walkThroughFiles(file, fun);
        }
        file.close();
    }
}

//recursively fill a fileArray with all files and folders on the FS
void addFolder(File folder, JsonArray fileArray)
{
	folder.rewindDirectory();
	while (true)
	{
		File file = folder.openNextFile();
		if (!file)
		{
			break;
		}
		else
		{
            JsonObject fileObject = fileArray.add<JsonObject>();
            fileObject["name"] = (char *)file.name(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
            fileObject["path"] = (char *)file.path(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
            fileObject["isFile"] = !file.isDirectory();
            // ESP_LOGI(TAG, "file %s (%d)", file.path(), file.size());
			if (file.isDirectory())
			{
				addFolder(file, fileObject["files"].to<JsonArray>());
			}
			else
			{
                fileObject["size"] = file.size();
                fileObject["time"] = file.getLastWrite();
			}
            // serializeJson(fileObject, Serial);
            file.close();
		}
	}
}

void FilesState::read(FilesState &state, JsonObject &root)
{
    root["name"] = "/";
    //crashes for some reason: ???
    root["fs_total"] = ESPFS.totalBytes() / 1000;
    root["fs_used"] = ESPFS.usedBytes() / 1000;
    File folder = ESPFS.open("/");
    addFolder(folder, root["files"].to<JsonArray>());
    folder.close();
    // print->printJson("FilesState::read", root);
    ESP_LOGI(TAG, "");
}

//utility function
void extractPath(const char *filepath, char *path) {
    const char *lastSlash = strrchr(filepath, '/');
    if (lastSlash != NULL) {
        size_t pathLength = lastSlash - filepath;
        strncpy(path, filepath, pathLength);
        path[pathLength] = '\0';
    } else {
        // No directory separator found, the entire filepath is the filename
        strcpy(path, "");
    }
}

StateUpdateResult FilesState::update(JsonObject &root, FilesState &state)
{
    state.updatedItems.clear();

    JsonArray deletes = root["deletes"].as<JsonArray>();
    if (!deletes.isNull()) {
        for (JsonObject var : deletes) {
            ESP_LOGI(TAG, "delete %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("new file", var);
            if (var["isFile"])
                ESPFS.remove(var["path"].as<const char*>());
            else
                ESPFS.rmdir(var["path"].as<const char*>());
            
            state.updatedItems.push_back(var["path"].as<const char*>());
        }
    }

    JsonArray news = root["news"].as<JsonArray>();
    if (!news.isNull()) {
        for (JsonObject var : news) {
            ESP_LOGI(TAG, "new %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("new file", var);
            if (var["isFile"]) {
                File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
                const char *contents = var["contents"];
                if (strlen(contents)) {
                    if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                        ESP_LOGE(TAG, "Write failed");
                    }
                }
                file.close();
            } else {
                ESPFS.mkdir(var["path"].as<const char*>());
            }
            state.updatedItems.push_back(var["path"].as<const char*>());
        }
    }

    JsonArray updates = root["updates"].as<JsonArray>();
    if (!updates.isNull()) {
        for (JsonObject var : updates) {
            ESP_LOGI(TAG, "update %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("update file", var);
            File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
            if (!file) {
                ESP_LOGE(TAG, "Failed to open file");
            }
            else {
                const char *contents = var["contents"];
                if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                    ESP_LOGE(TAG, "Write failed");
                }
                file.close();

                char newPath[64];
                extractPath(var["path"], newPath);
                strcat(newPath, "/");
                strcat(newPath, var["name"]);

                ESP_LOGI(TAG, "rename %s to %s", var["path"].as<const char*>(), newPath);

                if (strcmp(var["path"], newPath) != 0) {
                    ESPFS.rename(var["path"].as<const char*>(), newPath);
                }
                state.updatedItems.push_back(var["path"].as<const char*>());
            }
        }
    }

    if (state.updatedItems.size())
        ESP_LOGD(TAG, "first item %s", state.updatedItems.front().c_str());

    return state.updatedItems.size()?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

FilesService::FilesService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(FilesState::read,
                                                                                                         FilesState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/filesState",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FilesState::read,
                                                                                                          FilesState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "files"),
                                                                                           _webSocketServer(FilesState::read,
                                                                                                            FilesState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/filesState",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _server(server)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void FilesService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    onConfigUpdated();

    //setup the file server
    _server->serveStatic("/rest/file", ESPFS, "/");
}

void FilesService::onConfigUpdated()
{
    ESP_LOGI(TAG, "");
}

#endif