<?php
header("Cache-Control: private, no-store, no-cache, must-revalidate, max-age=0");
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");
date_default_timezone_set('Asia/Tokyo');
$remote = $_SERVER['REMOTE_ADDR'];
$arg_remote = escapeshellarg($_SERVER['REMOTE_ADDR']);
$lookup = '^'.date('YmdHi').'.. '.$remote.' ';
$arg_lookup = escapeshellarg($lookup);
if (trim(shell_exec('egrep '.$arg_lookup.' ../../log/submit.log')) !== '') {
    header ('HTTP/1.0 503 Service Temporarily Unavailable');
    include(dirname(__FILE__).'/503.html');
    exit;
}
?>
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<title>Brainhack Result</title>
<!--[if IE]>
<script src="http://html5shiv.googlecode.com/svn/trunk/html5.js"></script>

<![endif]-->
<style>
  article, aside, dialog, figure, footer, header, hgroup, menu, nav, section { display: block; }
</style>
</head>
<body>
<?php
$visualize = FALSE;
if (isset($_POST['team']) && trim($_POST['team']) !== '') {
    $team = trim($_POST['team']);
} else {
    $team = "\t";
}
$arg_team = escapeshellarg(base64_encode($team));
if (isset($_POST['code'])) {

    echo '<h1>Result</h1>'."\n";

    $code = base64_encode($_POST['code']);
    $size = strlen($_POST['code']);

    # logging
    $date = date('YmdHis');
    $arg_date = escapeshellarg($date);
    $arg_code = escapeshellarg($code);
    $arg_size = escapeshellarg($size);
    shell_exec('echo '.$arg_date.' '.$arg_remote.' '.$arg_team.' '.$arg_size.' '.$arg_code.' >> ../../log/submit.log');

    # judge the output
    $output = shell_exec('echo '.$arg_code.' | base64 -d | /usr/local/bin/brainhack');
    if (rtrim($output) === 'Hello, World!') {
        $visualize = TRUE;
        # scoreing
        if ($team !== "\t") {
            shell_exec('echo '.$arg_size.' '.$arg_team.' >> ../../log/score.txt');
        }

        echo '<p><b>Conglaturations!</b></p>'."\n";
        echo '<p>Your code size is ';
        echo htmlspecialchars($size, ENT_QUOTES, 'UTF-8');
        echo '.</p>'."\n";
        echo '<p><b>This attack flag is:</b></p>'."\n";
        echo '<p>';
        $flag = shell_exec('cat /attack1.txt');
        echo htmlspecialchars($flag, ENT_QUOTES, 'UTF-8');
        echo '</p>'."\n\n";
    } else {
        echo '<p><b>Failed.</b></p>'."\n";
        echo '<h1>Your Output</h1>'."\n";
        echo '<textarea readonly>';
        echo htmlspecialchars($output, ENT_QUOTES, 'UTF-8');
        echo '</textarea>'."\n";
        echo '<h1>Expected Output</h1>'."\n";
        echo '<textarea readonly>Hello, World!</textarea>'."\n\n";
    }
}
?>

<h1>Code Golf Ranking</h1>

<table border="1">
<tr><th>Rank</th><th>Score</th><th align="left">Name</th></tr>
<?php
function read_score()
{
    if ($score = fopen('../../log/score.txt', 'r')) {
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

$top = FALSE;
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
        if ($rank === 1 && $score == $size) $top = TRUE;
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
    echo htmlspecialchars($name, ENT_QUOTES, 'UTF-8');
    echo '</td>';
    echo '</tr>'."\n";
}
if ($visualize) {
    $view = $top ? 'success.sh' : 'fail.sh';
    shell_exec('/usr/local/bin/'.$view.' '.$arg_remote);
}
?>
</table>

</body>
</html>
