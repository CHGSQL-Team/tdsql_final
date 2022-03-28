rm tdsql.zip
rm -r pack
mkdir pack
zip -r pack/SOURCE.zip include/ src/ thirdparty/ extfetcher/src/ extfetcher/libs/ CMakeLists.txt
zip -r pack/TRD_SRCS.zip TRD_SRCS/
zip tdsql.zip pack/SOURCE.zip pack/TRD_SRCS.zip make.sh start.sh
rm -r pack
