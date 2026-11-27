#!/bin/sh

cp ../gim_gkfp/gkfp.cc .

doxygen

cp gkript_logo_site.png html/

cp *.conf html/
cp *.gkk html/
cp *.cc html/

#sudo rsync -av html/ /var/www/html

echo "Documentation done !"


