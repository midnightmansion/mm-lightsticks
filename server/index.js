import { WebSocketServer } from 'ws';

const wss = new WebSocketServer({ port: 8080 });

const pixelCount = 38;

wss.on('connection', function connection(ws) {
  ws.on('message', function message(data) {
    console.log('received: %s', data);
  });

  console.log('received connection');

  setInterval(() => {
    const hexValues = [...Array(pixelCount * 3)].map(() => {
      const hexValue = Math.floor(Math.random() * 90).toString(16);
      return hexValue.length === 1 ? `0${hexValue}` : hexValue;
    });
    const buffer = Buffer.from(hexValues.join(''), 'hex');
    ws.send(buffer);
    console.log('Sent all hex values:', hexValues.join(' '));
  }, 1);
});
