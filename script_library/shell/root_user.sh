#!/bin/bash

# check if user is root or not
if [ $UID -eq 0 ]; then
    echo "Root user"
else
    echo "Not root user"
fi
