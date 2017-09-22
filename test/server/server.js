/* jslint node: true */
'use strict';

const paths         = require('path');
const SocketCluster = require('socketcluster').SocketCluster;

const serverOptions = {
    workers             : 1,
    brokers             : 1,
    port                : 8000,
    appName             : 'scio_beast test server',
    workerController    : paths.join(__dirname, 'worker.js'),
    protocol            : 'http',
    perMessageDeflate   : true,
};

const socketCluster = new SocketCluster(serverOptions);
