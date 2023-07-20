import { WebSocketServer } from 'ws';

const wss = new WebSocketServer({
  port: 8080,
});

const connections = {};

const ipToTubeNumberMapping = {
  32: 0,
  33: 1,
  2: 2,
  3: 3,
  4: 4,
  5: 5,
  6: 6,
  7: 7,
}

setInterval(() => {
  console.log('Connections list is now:', Object.keys(connections));
}, 5000);

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

  let ip;

  ws.on('error', function error(err) {
    console.log('Error: %s', err);
    console.log('disconnecting: %s', ip);
    delete connections[ip];
    console.log('Connections list is now:');
    console.log(Object.keys(connections));
  });

  // initially set all connections to output
  ws.on('close', function close() {
    console.log('disconnecting: %s', ip);
    delete connections[ip];
    console.log('Connections list is now:');
    console.log(Object.keys(connections));
  });

  ws.on('message', function message(data) {
    if (isIP(data)) {
      console.log('New Connection with IP: %s', data);
      ip = data;
      connections[data] = {
        type: "output",
        ws
      }
      console.log('Connections list is now:');
      console.log(Object.keys(connections));
    }

    try {
      const videoData = JSON.parse(data);
      if (videoData.type === "source") {
        ip = 'APP';
        if (connections[ip] === undefined) {
          console.log('New Source Connection from Web Browser: %s', ip);
          connections[ip] = {
            type: "source",
          }
          console.log('Connections list is now:');
          console.log(Object.keys(connections));
        }

        for (const connection in connections) {
          if (connections[connection].type === "output") {
            const ipLastOctet = connection.split('.')[3];
            // get first 111 elements of videoData
            // just the first row

            const firstColumn = videoData.data.filter((value, index) => {
              return index % 8 === ipToTubeNumberMapping[ipLastOctet];
            }).flat();
            const hexValues = firstColumn.map((value) => {
              return getHex(value);
            });

            // send IP address ending in 02 (x.x.x.2)
            // TODO zerofill
            const buffer = Buffer.from([ipLastOctet.toString(), ...hexValues].join(''), 'hex');
            connections[connection].ws.send(buffer);
          }
        }
      }
    } catch (e) {
      // console.log('Output Connection attempting to send data');
    }
  });
});
