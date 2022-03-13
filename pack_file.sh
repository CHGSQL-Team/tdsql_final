rm SOURCE.zip
rm tdsql.zip
zip -r SOURCE.zip include/ src/ thirdparty/ CMakeLists.txt
zip tdsql.zip SOURCE.zip make.sh start.sh
rm SOURCE.zip