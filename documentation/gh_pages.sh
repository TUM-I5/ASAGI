#!/bin/bash

# SPDX-License-Identifier: LGPLv3
# 
# SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>

export REPO="$(pwd | sed s,^/home/travis/build/,,g)"
echo -e "Current Repo:$REPO --- Travis Branch:$TRAVIS_BRANCH" 

git config --global user.email "rettenbs@in.tum.de"
git config --global user.name "Travis" 

if [ "$TRAVIS_BRANCH" == "master" ]; then
	git clone --quiet --branch=gh-pages https://${GH_TOKEN}@github.com/TUM-I5/ASAGI.git gh-pages > /dev/null
	cd gh-pages
	
	git rm -rf .
	cp -r ../build/documentation/devdoc/* .
	cp ../build/documentation/doc.pdf .
	git add -f .
	git commit -m "Travis build $TRAVIS_BUILD_NUMBER pushed to gh-pages"
	git push -fq origin gh-pages > /dev/null
fi 