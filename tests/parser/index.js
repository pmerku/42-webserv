const net = require('net');
const sleep = (ms) => new Promise((r) => setTimeout(()=>r(), ms));

const port = 8080;

function doRequestPackets(arr) {
    return new Promise((resolve, reject) => {
        const s = net.Socket();
        let str = "";
        const send = (d) => {
            s.write(d);
        }
        s.on('data', (d) => {str += d.toString()});
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

        s.connect(port);
    })
}

function doRequestRaw(str) {
    return doRequestPackets([str]);
}

function httpRequest(ops = {}) {
    const req = {
        method: "GET",
        uri: "/",
        version: "1.1",
        headers: [ "Host: localhost" ],
        body: "",
        ...ops,
    }
    if (ops.addHeaders) {
        req.headers = [ ...req.headers, ...ops.addHeaders ];
    }
    return `${req.method} ${req.uri} HTTP/${req.version}\r\n${req.headers.map(v=>v+"\r\n").join("")}\r\n${req.body}`;
}

const tests = [
    ["GET / HTTP/1.1\r\n\r\n", 400], // no header
    [["GET /", " HTTP/1", ".1\r\nHost: localhost\r\n\r\n"], 200], // send individual packets
    ["GET  / HTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid status line
    ["GET /  HTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid status line
    ["GET /\tHTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid status line
    ["GET /HTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid status line
    ["LOL / HTTP/1.1\r\n\r\n", 501], // wrong HTTP method
    ["GET / HTTP/1.0\r\n\r\n", 505], // wrong HTTP version
    ["GET abc HTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid uri (no slash)
    ["GET /^^^^Helloworld HTTP/1.1\r\nHost: localhost\r\n\r\n", 400], // invalid uri chars
    [httpRequest(), 200], // normal
    [httpRequest({ headers: ["Hello: World" ] }), 400], // no host header
    [httpRequest({ addHeaders: ["Content-length: 123456789056789023462345234563"]} ), 413], // length overflow
    [httpRequest({ addHeaders: ["Content-length: 1234"]} ), 413], // too much body
    [httpRequest({ addHeaders: ["I-existValuehere"]} ), 400], // invalid header
    [httpRequest({ addHeaders: ["I-exist : Valuehere"]} ), 400], // invalid header field name
    [httpRequest({ addHeaders: ["I-exist: "+("42".repeat(4000))]} ), 431], // header payload too large
    [httpRequest({ addHeaders: ["Content-Length: 12", "Transfer-encodING:      chunked"]} ), 400], // encoding + length
]

function runTest(test) {
    if (test.constructor == String)
        return doRequestRaw(test);
    else
        return doRequestPackets(test);
}

(async () => {
    let success = 0;
    let count = tests.length;
    for (let i = 0; i < count; i++) {
        try {
            const timeStart = (new Date()).getTime();
            const result = await runTest(tests[i][0]);
            const timeDiff = (new Date()).getTime() - timeStart;
            const status = parseInt(result.replace("HTTP/1.1 ", "").substr(0, 3));
            if (status == tests[i][1]) {
                console.log(`\u001b[32mTest ${i}: Success after ${timeDiff}ms (${status})\u001b[0m`);
                success++;
            } else
                console.log(`\u001b[31mTest ${i}: Failed (${status})\u001b[0m\n`, result);
        } catch (err) {
            console.log(`\u001b[31mTest ${i}: Failed (ERROR)\u001b[0m\n`, err);
        }
    }
    console.log("");
    if (success == count)
        console.log(`\u001b[32mTests ran succesfully (${success}/${count})\u001b[0m`);
    else
        console.log(`\u001b[31mSome tests failed! (${success}/${count})\u001b[0m`);
})()
