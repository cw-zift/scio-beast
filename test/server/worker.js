/* jslint node: true */
'use strict';

function logEvent(name, eventData) {
	console.log(`${name}: ${JSON.stringify(eventData)}`);
}

const respGotIt = { got_it : true };

module.exports.run = function(worker) {
	console.log(`Worker startup with PID: ${process.pid}`);

	const scServer = worker.scServer;

	scServer.on('connection', socket => {

		socket.on('event_no_resp', eventData => {
			logEvent('event_no_resp', eventData);
		});

		socket.on('event_with_resp', (eventData, resp) => {
			logEvent('event_with_resp', eventData);

			return resp(null, respGotIt);
		});

		socket.on('event_with_timed_out_resp', (eventData, resp) => {
			logEvent('event_with_timed_out_resp', eventData);

			setTimeout( () => {
				return resp(null, respGotIt);
			}, eventData.ackTimeout + 5000);
		});
	});
}
