#!/bin/bash

D=`date +%Y-%m-%d`

cd /backup/fritz/tmp

#google code
hg clone https://progressive-learning-platform.googlecode.com/hg/ plp-reference-code
hg clone https://wiki.progressive-learning-platform.googlecode.com/hg plp-reference-wiki
#hg clone https://okstate-ecen4243-plp-spring-2011.googlecode.com/hg/ plp-sp2011-code
#hg clone https://wiki.okstate-ecen4243-plp-spring-2011.googlecode.com/hg/ plp-sp2011-wiki

#ROME data
mkdir website
scp -r fritz@rome.ceat.okstate.edu:~/plp ./website/plp
scp -r fritz@rome.ceat.okstate.edu:~/public_html ./website/public_html

#google docs
#doesn't quite work yet... I'll have to do this one manually

tar -cjf /backup/fritz/nightly/${D}-nightly.tar.bz2 *

rm -rf *

