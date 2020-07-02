@echo off

echo ********** RUN CMAKE **********


mkdir Build
cd Build
call cmake .. || goto :error


echo ********** PROCESS COMPLETED **********


exit /b

:error
echo Aborting script due to error.
exit /b