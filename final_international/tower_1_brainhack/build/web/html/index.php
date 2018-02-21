<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<title>Brainhack</title>
<!--[if IE]>
<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>

<![endif]-->
<style>
  article, aside, dialog, figure, footer, header,
hgroup, menu, nav, section { display: block; }
</style>
</head>
<body>

<h1>Brainhack</h1>

<p>
This is a <a href="https://en.wikipedia.org/wiki/Brainfuck">Brainf*ck</a> compatible interpreter service.<br/>
<p/>
<p>
Your team can get an attack flag if you submit a program code that output "Hello, World!".<br/>
Your team can get defense points if you submit the shortest program code that output "Hello, World!".<br/>
But, you can submit it once a minute. So, you should check the output on your PC using <a href="/download/brainhack">the Brainhack binary</a> for Ubuntu16.04 64bit.
</p>
<p>
NOTE: Whitespace characters at the end of the output are ignored.
</p>
<p/>
<p/>

<h1>Submit Form</h1>

<form action="/cgi-bin/check.php" method="post">

<p><label>Your Team Keyword:<br>
<input type="text" name="team">
</label></p>

<p><label>
Your Brainhack Code:<br>
<textarea name="code" cols="80" rows="8"></textarea>
</label></p>

<p>
<input type="submit" value="Submit">
</p>
</form>

<p/>

<h1>Code Golf Ranking</h1>

<table border="1">
<tr><th>Rank</th><th>Score</th><th align="left">Name</th></tr>
<?
$rank = 0;
$last = '';
$same = 0;
exec('cat ../log/score.txt', $lines, $ret);
$lines = array_unique($lines);
natsort($lines);
foreach (array_slice($lines, 0, 200) as $line) {
    list($score, $name) = explode(' ', $line, 2);
    if (strlen($score) > 8) {
	continue;
    }
    if (strlen(base64_decode($name)) == 0) {
	continue;
    }
    if ($score === $last) {
        $same++;
    } else {
        $rank = $rank + $same + 1;
        $same = 0;
        $last = $score;
    }
    echo '<tr>';
    echo '<td align="right">';
    echo htmlspecialchars($rank, ENT_QUOTES, 'UTF-8');
    echo '</td>';
    echo '<td align="right">';
    echo htmlspecialchars($score, ENT_QUOTES, 'UTF-8');
    echo '</td>';
    echo '<td align="left">';
    echo htmlspecialchars(base64_decode($name), ENT_QUOTES, 'UTF-8');
    echo '</td>';
    echo '</tr>'."\n";
}
?>
</table>

</body>
</html>
