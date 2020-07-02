@echo off


echo ********** STARTING WEBSERVER **********


cd Build_Web
call python -m http.server
pause