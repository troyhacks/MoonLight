/**
    @title     MoonBase
    @file      FileManager.cpp
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonbase/FileManager/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact us for more information.
**/

#if FT_MOONBASE == 1

#include "FileManager.h"

#include "Utilities.h"

//recursively fill a fileArray with all files and folders on the FS
void addFolder(File folder, bool showHidden, JsonArray fileArray)
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
            if (showHidden || file.name()[0] != '.') {
                JsonObject fileObject = fileArray.add<JsonObject>();
                fileObject["name"] = (char *)file.name(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
                fileObject["path"] = (char *)file.path(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
                fileObject["isFile"] = !file.isDirectory();
                // MB_LOGI(MB_TAG, "file %s (%d)", file.path(), file.size());
                if (file.isDirectory())
                {
                    addFolder(file, showHidden, fileObject["files"].to<JsonArray>());
                }
                else
                {
                    fileObject["size"] = file.size();
                    fileObject["time"] = file.getLastWrite();
                }
                // serializeJson(fileObject, Serial);
            }
            file.close();
		}
	}
}

void FilesState::read(FilesState &state, JsonObject &root)
{
    root["name"] = "/";
    //crashes for some reason: ???
    root["fs_total"] = ESPFS.totalBytes();
    root["fs_used"] = ESPFS.usedBytes();
    root["showHidden"] = state.showHidden;
    File folder = ESPFS.open("/");
    addFolder(folder, state.showHidden, root["files"].to<JsonArray>());
    folder.close();
    // print->printJson("FilesState::read", root);
    MB_LOGI(MB_TAG, "");
}

StateUpdateResult FilesState::update(JsonObject &root, FilesState &state)
{
    bool changed = false;

    if (root["showHidden"] != state.showHidden) {
        state.showHidden = root["showHidden"];
        MB_LOGD(MB_TAG, "showHidden %d", state.showHidden);
        changed = true;
    }

    state.updatedItems.clear();

    JsonArray deletes = root["deletes"].as<JsonArray>();
    if (!deletes.isNull()) {
        for (JsonObject var : deletes) {
            MB_LOGI(MB_TAG, "delete %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
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
            MB_LOGI(MB_TAG, "new %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("new file", var);
            if (var["isFile"]) {
                File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
                const char *contents = var["contents"];
                if (strlen(contents)) {
                    if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                        MB_LOGE(MB_TAG, "Write failed");
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
            MB_LOGI(MB_TAG, "update %s %s", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("update file", var);
            File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
            if (!file) {
                MB_LOGE(MB_TAG, "Failed to open file");
            }
            else {
                const char *contents = var["contents"];
                if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                    MB_LOGE(MB_TAG, "Write failed");
                }
                file.close();

                char newPath[64];
                extractPath(var["path"], newPath);
                strcat(newPath, "/");
                strcat(newPath, var["name"]);

                MB_LOGI(MB_TAG, "rename %s to %s", var["path"].as<const char*>(), newPath);

                if (strcmp(var["path"], newPath) != 0) {
                    ESPFS.rename(var["path"].as<const char*>(), newPath);
                }
                state.updatedItems.push_back(var["path"].as<const char*>());
            }
        }
    }

    changed |= state.updatedItems.size();

    if (changed && state.updatedItems.size())
        MB_LOGV(MB_TAG, "first item %s", state.updatedItems.front().c_str());

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

FileManager::FileManager(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(FilesState::read,
                                                                                                         FilesState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/FileManager",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FilesState::read,
                                                                                                          FilesState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "FileManager"),
                                                                                           _webSocketServer(FilesState::read,
                                                                                                            FilesState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/FileManager",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
                                                                                             _server(server),
                                                                                             _sveltekit(sveltekit)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void FileManager::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    onConfigUpdated();

    //setup the file server
    _server->serveStatic("/rest/file", ESPFS, "/");

    _server->on("/rest/saveConfig", HTTP_POST, _sveltekit->getSecurityManager()->wrapRequest( [this](PsychicRequest *request)
    {
        request->reply(200);

        FSPersistence<int>::writeToFSDelayed('W'); //write all delayed writes to the FS

        saveNeeded = false;

        return ESP_OK;
    }, AuthenticationPredicates::IS_AUTHENTICATED));
    
    _server->on("/rest/cancelConfig", HTTP_POST, _sveltekit->getSecurityManager()->wrapRequest( [this](PsychicRequest *request)
    {
        request->reply(200);
        FSPersistence<int>::writeToFSDelayed('C'); //read back from FS
        //update UI...

        saveNeeded = false;

        return ESP_OK;
    }, AuthenticationPredicates::IS_AUTHENTICATED));


}

void FileManager::onConfigUpdated()
{
    MB_LOGI(MB_TAG, "");
}

#endif