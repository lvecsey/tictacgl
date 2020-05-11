Connect a set of nodes by IP:PORT and play a game of tictactoe.

Place your node ids, and ip address with ports in processes.txt

```console
0->ad.dr.es.ss:5346
1->ad.dr.es.ss:5346
2->ad.dr.es.ss:5346
```

Run as ./tictacgl ./processes.txt bind_ip:port public_ip:port

The IP and port to bind as can be the same as the public ip and port, except if you only have a private IP on your machine. In that case use it as your bind ip and make sure you have port forwarding and your firewall set up to forward traffic to the private ip.

The program will look through processes.txt and determine which process id corresponds to the public ip and port specified for it.

You take turns making a move, clicking on a square with the left mouse button. Anyone in the peer network can make the next move, so long as they haven't moved last.

The *backspace* key will save an .rgb screenshot image. You can view it with:

```console
display -size 1920x1080 tictacgl_screenshot.rgb
```

Press *space bar* to clear the screen and restart the game, or *Escape* to exit.

![Image of output](https://phrasep.com/~lvecsey/software/tictacgl/tictacgl_screenshot.png)

