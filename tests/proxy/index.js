var net = require('net');

var server = net.createServer(function(socket) {
    let str = "";
    socket.on("data", (d) => {
        str+=d.toString();
        console.log(str);
        if (str.includes("\r\n\r\n")) {
            socket.write("This is a response!!");
            socket.destroy();
        }
    })
});

server.listen(1337, '127.0.0.1');