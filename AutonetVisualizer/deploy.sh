#!/usr/bin/env bash

########################################
#              Deploy                  #
########################################

#This script will the autonet client on the current branch
#to the heroku host

# Be sure to run 'grunt build' before and commit any changes
# to the 'dist' directory that result


# Go to root git directory
cd "$(git rev-parse --show-toplevel)"

# Get current branch name
branch=`git rev-parse --abbrev-ref HEAD`
echo "Deploying branch '$branch' to heroku"

# Push AutonetVisualizer as subtree to heroku
git push heroku `git subtree split --prefix="source/Autowiring/AutonetVisualizer" $branch`:master
