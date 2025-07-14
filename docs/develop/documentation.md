## Share Knowledge - Improve Documentation

A software project includes its functional aspects and the knowledge surrounding it. Document your experience to make it easier to have fun making things and to get engaged in improving the project.

### MKDocs Preview in VS Code - Installation

This method can be used to see a live preview of changes to the docs while editing in VS Code.

Download the ZIP: [https://github.com/libukai/mkdocs-preview](https://github.com/libukai/mkdocs-preview)

Place the following in workspace settings (F1, if working in a workspace) or create .vscode/settings.json:

	{
		"mkdocsPreview.port": 8000,
		//"mkdocsPreview.port": 4001,
		
		// remove the '/docs' prefix from every URL
		// rewrite "/docs/XYZ" → "/XYZ"
		"mkdocsPreview.baseDir": "",
		"mkdocsPreview.urlReplace": "/docs/||/",

		"simpleBrowser.defaultUrl": "http://localhost:8000/"
		//"simpleBrowser.defaultUrl": "http://127.0.0.1:8000/",
	}

Open Powershell in your working directory in VS Code.

Run:

	python --version
	python -m pip --version
	python -m pip install --upgrade pip
	python -m pip install mkdocs-material

Run either:

	mkdocs serve -a localhost:8000
	mkdocs serve -a 127.0.0.1:8000

It should now be visible 🐉
	

####Troubleshooting:
1. Try using these commands to manually test addresses after starting the service in VS Code:

	Ctl + Shft + P -> MKDocs Preview: Toggle MkDocs Preview

	Ctl+ Shft + P -> Simple Browser: Show

2. Check the address in a browser:

	[https://localhost:8000](https://localhost:8000)

	[https://127.0.0.1:8000/](https://127.0.0.1:8000/)

3. Ensure mkdocs serve is running and reboot the service.