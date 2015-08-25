#!/bin/bash

ps aux | grep kgi_plugin_fix_svr  | grep -v grep | awk '{print "kill "$2}' | sh

