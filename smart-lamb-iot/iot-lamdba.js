'use strict';

var AWS = require('aws-sdk');
var iotdata = new AWS.IotData({ endpoint: 'a18fzgq0yfpfy9.iot.ap-southeast-1.amazonaws.com' });

console.log('--- Loading function ---');

exports.handler = (event, context, callback) => {
  //console.log('Received event:', JSON.stringify(event, null, 2));
  console.log('value1 =', event.key1);
  console.log('free_ram =', event.free_ram);
  console.log('total_ram =', event.total_ram);
  callback(null, event.key1); // Echo back the first key value
  //callback('Something went wrong');

  sendToIot();
  testExternalRequest();
};

var sendToIot = () => {
  var params = {
    topic: 'my/topic/1',
    payload: 'dam hong linh ahihihlah',
    qos: 0
  };

  iotdata.publish(params, function(err, data) {
    if (err) {
      console.log(err);
    } else {
      console.log("success?");
      //context.succeed(event);
    }
  });
}

var testExternalRequest = () => {
  var request = require('request');
  request('http://www.google.com', function (error, response, body) {
    console.log('error:', error); // Print the error if one occurred
    console.log('statusCode:', response && response.statusCode); // Print the response status code if a response was received
    console.log('body:', body); // Print the HTML for the Google homepage.
  });
}
