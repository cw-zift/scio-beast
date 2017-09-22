/* jslint node: true */
'use strict';

const paths			= require('path');
const SocketCluster = require('socketcluster').SocketCluster;
//const fs			= require('fs');

const serverOptions = {
	workers				: 1,
	brokers				: 1,
	port				: 8000,
	appName				: 'scio_beast test server',
	workerController	: paths.join(__dirname, 'worker.js'),
	protocol			: 'http',

	perMessageDeflate	: true,

/*
	authPrivateKey		: fs.readFileSync(paths.join(__dirname, 'private.pem'), 'utf8'),
	authPublicKey		: fs.readFileSync(paths.join(__dirname, 'public.pem'), 'utf8'),
	authAlgorithm		: 'RS256',	
*/	
};

const socketCluster = new SocketCluster(serverOptions);
