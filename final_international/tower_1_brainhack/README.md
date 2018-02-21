# SECCON 2017 Final International CTF Tower#1 "brainhack"

### Genre

programming + pwn

### Question

<pre>
http://10.0.21.89:8989/

The system specification:
    docker host: Ubuntu 16.04 TLS
    docker container: nginx:1.13.8-alpine
    docker container: php:7.2.1-fpm (read-only container and append only files)
    docker-compose version 1.18.0, build 8dd22a9
</pre>

### The Web Page

<blockquote>
<p>
This is a <a href="https://en.wikipedia.org/wiki/Brainfuck">Brainf*ck</a> compatible interpreter service.<br>
</p>
<p>
Your team can get an attack flag if you submit a program code that output "Hello, World!".<br>
Your team can get defense points if you submit the shortest program code that output "Hello, World!".<br>
But, you can submit it once a minute. So, you should check the output using the Brainhack binary on your PC.<br>
</p>
<p>
NOTE: Whitespace characters at the end of the output are ignored.
</p>

#### Submit Form

<p><label>Your Team Keyword:<br>
[textfield]
</label></p>

<p><label>Your Brainhack Code:<br>
[textarea]<br>
</label></p>

Submit[button]

#### Code Golf Ranking

| Rank | Name | Code Size |
|-----:|:-----|:-----|
| 1 | SECCON | 79 |
</blockquote>

### How to build the server

<p>
It runs on docker using docker_compose supporting compose file version 3.<br>
So, you need to prepare the environment.<br>
After that, you can build and run it:<br>

<pre>
$ cd final_international/tower_1_brainhack/build
$ make up
</pre>
</p>
