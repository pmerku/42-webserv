const net = require('net');
const sleep = (ms) => new Promise((r) => setTimeout(()=>r(), ms));

const port = 80;

function doRequestPackets(arr) {
    return new Promise((resolve, reject) => {
        const s = net.Socket();
        let str = "";
        const send = (d) => {
            s.write(d);
        }
        s.on('data', (d) => {str += d.toString(); console.log(str)});
        s.on('close', () => {
            resolve(str);
        })

        s.on('connect', async () => {
            for (let data of arr) {
                send(data);
                await sleep(100);
            }
        })

        s.on('error', (e) => {
            reject(e);
        })

        s.connect(port, "66.228.62.75");
    })
}

function doRequestRaw(str) {
    return doRequestPackets([str]);
}

(async () => {
    const data = await doRequestRaw("GET / HTTP/1.1\r\nContent-Length: 0\r\nDate: Mon, 05 April 2021 16:39:19 GMT\r\nHost: designcourse.com\r\nX-Forwarded-For: 192.168.0.1\r\nX-Forwarded-Host: localhost:8080\r\nX-Forwarded-Proto: http\r\n\r\n");
    console.log(data)
    console.log("end of request");
})()