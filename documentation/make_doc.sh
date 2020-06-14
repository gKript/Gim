#!/bin/sh

cp ../gim_gkfp/gkfp.cc .

doxygen

cp Logo_gKript_documentazione.png html/
cp Logo_gKript_documentazione_small.png html/

cp *.conf html/
cp *.gkk html/
cp *.cc html/

#sudo rsync -av html/ /var/www/html

echo "Documentation done !"


