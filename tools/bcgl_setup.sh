#!/bin/bash

echo "--- BCGL Setup ---"

read -p "Do you want to create a new project in the current directory (y/n)? " yn
if [ $yn != "y" ] && [ $yn != "Y" ]; then
    exit
fi

echo
echo "Project info"
read -p "Name: " name
read -p "Title (default: same as name):" title
read -p "Company (default: com.bcgl):" company
read -p "Do you want to initiate a new git project (y/n)? " git_init

if [ -z $title ]; then title=$name; fi
if [ -z $Company ]; then Company="com.bcgl"0; fi

echo "--- New project info ---"
echo "Name = $name"
echo "Title = $title"
echo "Company = $company"
echo "Init git = $git_init"
read -p "Do you want to create a this project (y/n)? " yn
if [ $yn != "y" ] && [ $yn != "Y" ]; then
    exit
fi

git clone git@gitlab.com:ilijabc/bcgl.git
if [ $yn == "y" ] || [ $yn == "Y" ]; then
    git init
fi
python bcgl/tools/create_project.py -N $name -T $title -C $company

echo "Prject ready."
