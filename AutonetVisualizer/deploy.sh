#!/usr/bin/env bash

########################################
#              Deploy                  #
########################################

#This script will the autonet client on the current branch
#to the heroku host

# Be sure to run 'grunt build' before and commit any changes
# to the 'dist' directory that result


# Check if there are any uncommited changes
#if [ $(git status --porcelain 2>/dev/null| egrep "^(M| M)" | wc -l) ]
#then
#  echo -e "\nCan't deploy!!!\n"
#  echo "Uncommit changes. Be sure to run 'grunt build' and commit changes to 'dist' directory"
#  exit
#fi

echo "Updating deploy date...."
heroku config:set DEPLOY_DATE="`date`" --app autonet
echo "done"

# Go to root git directory
cd "$(git rev-parse --show-toplevel)"

# Get current branch name
branch=`git rev-parse --abbrev-ref HEAD`
echo "Deploying branch '$branch' to heroku"

# Push AutonetVisualizer as subtree to heroku
git push heroku `git subtree split --prefix="source/Autowiring/AutonetVisualizer" $branch`:master --force
