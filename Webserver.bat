@echo off


echo ********** STARTING WEBSERVER **********


cd Build
call python -m http.server
pause