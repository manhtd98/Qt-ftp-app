### GUI Qt5 Application for Browser FTP client
### Introduction

This Qt FTP client application allows users to connect to an FTP server and perform various file operations such as listing files, downloading, uploading, removing files, and adding directories.

### Dependencies
Qt 5.15 or later

#### Library

- QtcpSocket: TCP Socket connect to FTP server
- QtAplication: GUI application.
- QFile: File transfer

### UML
![chart](mermaid-flow.png)
### Features
#### Connect to FTP Server
- Enter FTP server details (server address, username, password) in MainWindow.
- Click "Connect" to open FileView.
#### List Files and Directories
- In FileView, Click on directory that want to browser.
#### Download File
- In FileView, Double click the file on list file or directory. 
#### Upload File
- In FileView, click "Choose file" to select a file from your local system and upload it to the FTP server.
- Click "Upload" to start upload file
#### Remove File and directory
- In FileView, choose the file or directory
- Click button Remove
#### Add Directory
- In FileView, click "Add Directory" to create a new directory on the FTP server.
- Type the directory name and click "OK".

#### Disconnect
- In FileView, click "Disconnect".


### Developed by
- manhtd98
