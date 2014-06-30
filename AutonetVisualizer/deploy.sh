#!/usr/bin/env bash

########################################
#              Deploy                  #
########################################

#This script will the autonet client on the current branch
#to the heroku host


# Go to root git directory
cd "$(git rev-parse --show-toplevel)"

# Get current branch name
branch=`git rev-parse --abbrev-ref HEAD`
echo "Deploying branch '$branch' to heroku"

git push heroku `git subtree split --prefix="source/Autowiring/AutonetVisualizer" $branch`:master
