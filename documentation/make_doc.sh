#!/bin/sh

cp ../gim_gkfp/gkfp.cc .

doxygen

cp Logo_gKript_documentazione.png html/
cp Logo_gKript_documentazione_small.png html/

sudo rsync -av html/ /var/www/html

echo "Documentation done !"


