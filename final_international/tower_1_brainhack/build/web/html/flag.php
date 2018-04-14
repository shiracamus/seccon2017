<?php
function read_score()
{
    if ($score = fopen('../log/score.txt', 'r')) {
        while (!feof($score)) {
            $line = fgets($score);
            if (preg_match('/^[^\s]{1,8} [\w+\/]{1,128}={0,2}$/', $line))
                yield $line;
        }
        fclose($score);
    }
}

$lines = iterator_to_array(read_score());
natsort($lines);

$rank = 0;
$last = '';
$same = 0;
foreach ($lines as $line) {
    list($score, $name) = explode(' ', $line, 2);
    $name = trim(base64_decode($name));
    if (strlen($name) <= 0 || ! ctype_print($name)) continue;
    if ($score === $last) {
        $same++;
    } else {
        $rank = $rank + $same + 1;
        if ($rank > 1) exit;
        $same = 0;
        $last = $score;
    }
    if (preg_match('/^[a-fA-F0-9]{16,32}$/', $name)) {
        echo $name."\n";
    }
}
?>
