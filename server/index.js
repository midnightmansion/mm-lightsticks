import { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 8080 });

const pixelCount = 111;

const connections = {};

const getHex = (value) => {
  const hexValue = (Math.round(parseInt(value) / 2)).toString(16);
  return hexValue.length === 1 ? `0${hexValue}` : hexValue;
}

// function to check if data is the format of an IP address
const isIP = (data) => {
  const ipRegex = /(\d{1,3}\.){3}\d{1,3}/;
  return ipRegex.test(data);
}

wss.on('connection', function connection(ws) {

  // eventually we want to use something like IP address
  const uuid = Math.random().toString(36).substring(2, 15) + Math.random().toString(36).substring(2, 15);

  // initially set all connections to output
  connections[uuid] = {
    type: "output",
    ws
  }

  ws.on('close', function close() {
    delete connections[uuid];
    console.log('disconnected');
  });

  ws.on('message', function message(data) {
    if (isIP(data)) {
      console.log('IP: %s', data);
    }

    try {
      const videoData = JSON.parse(data);
      if (videoData.type === "source") {
        connections[uuid].type = "source";

        for (const connection in connections) {
          if (connections[connection].type === "output") {

            // get first 111 elements of videoData
            // just the first row
            const firstColumn = videoData.data.filter((value, index) => {
              return index % 8 === 0
            }).flat();
            const hexValues = firstColumn.map((value) => {
              return getHex(value);
            });
            const buffer = Buffer.from(['02', ...hexValues].join(''), 'hex');
            connections[connection].ws.send(buffer);
          }
        }
      }
    } catch (e) {
      console.log('Output Connection attempting to send data');
    }
  });

  // setInterval(() => {
  //   const hexValues = [...Array(pixelCount * 3)].map(() => {
  //     const hexValue = Math.floor(Math.random() * 90).toString(16);
  //     return hexValue.length === 1 ? `0${hexValue}` : hexValue;
  //   });
  //   const buffer = Buffer.from(hexValues.join(''), 'hex');
  //   // ws.send(buffer);
  //   // console.log('Sent all hex values:', hexValues.join(' '));
  // }, 50);

});
