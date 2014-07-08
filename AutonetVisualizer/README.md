# Autonet Visualizer Client

## Overview
This is a statically served webapp that listens to a localhost websocket server from the Autonet Library. The most recent version is hosted at http://autonet.herokuapp.com/

## Setup
This application was build using [Yeoman](http://yeoman.io/) for scaffolding, [Grunt](http://gruntjs.com/) for building, and [Bower](http://bower.io/) for javascript dependencies. You'll need to install these along with [npm](https://www.npmjs.org/) to run this project.

Once these tools are installed, you need to install the server dependencies specified in `package.json` with `npm install` and client libraries specified in `bower.json` with `bower install`.

To run a development server, simply run `grunt serve`. This will create a live updating page to test the client

## Deploying to Heroku
First, be sure you have the [Heroku toolbelt](https://toolbelt.heroku.com/) installed. You'll also need to set up your credentials for Leap Motion's heroku account.

Next, add the heroku remote to your local git repository. This can be done with `git remote add heroku git@heroku.com:autonet.git`

To deploy to Heroku, first run `grunt build` to create the distrution version in the `dist` directory. Next, commit any changes to `dist` and then run `deploy.sh`. This will push the subtree to heroku master
