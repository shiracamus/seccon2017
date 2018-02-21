<?php
$rank = 0;
$last = '';
$same = 0;
exec('cat ../log/score.txt', $lines, $ret);
natsort($lines);
foreach ($lines as $line) {
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
    if ($rank != 1) {
	exit;
    }
    $name = trim(base64_decode($name));
    if (preg_match('/^[a-fA-F0-9]{16,32}$/', $name)) {
	echo $name."\n";
    }
}
?>
