# Websocket Notification Server  /WIP/  

This directory contains a simple Websocket server which can connect to
a GSP built with libspex.  It uses the `waitforchange` RPC method
to poll the GSP for updates, and pushes notifications to all connected
clients whenever a new best block is found.
