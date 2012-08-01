<?php echo $this->Html->script('prettify/prettify.js'); ?>
<?php echo $this->Html->css('prettify.css'); ?>
<script type="text/javascript">
$(document).ready(function(){
    // Load prettify
    prettyPrint();

    // Handle example tabs
    $('#sources_examples a').click(function (e) {
	e.preventDefault();
	$(this).tab('show');
    })
    $('#satellites_examples a').click(function (e) {
	e.preventDefault();
	$(this).tab('show');
    })
    $('#error_examples a').click(function (e) {
	e.preventDefault();
	$(this).tab('show');
    })
	
    $(".expand_link").click(function (){
	row_id = $(this).attr('rel');
	link_content = $(this).html();
		
	if (link_content == '[- Satellites]'){
	    $(this).html('[+ Satellites]');
	    $("#source_"+row_id).css("background-color","white");
	} else {
	    $(this).html('[- Satellites]');
	    $("#source_"+row_id).css("background-color","#F5F5F5");
	}
	
	$("#satellites_"+row_id).toggle(400);
    });
});
</script>
<style type="text/css">
p {
    line-height: 1.5;
    padding-bottom: 5px;
}
.nav-tabs > .active > a {
    color: #555555;
    cursor: default;
    text-decoration: none;
}
.nav > li > a {
    color: #224479;
    text-decoration: none;
}


.expand_link {
    font-size: 10px;
    color: #3F3E3E;
    cursor: pointer;
}
.expand_link:hover {
    color: #6E6C6C;
}
.satellite_table {
    font-size: 10px;
}
.satellite_table td {
    background-color: white !important;
    padding: 2px 2px;
}
.satellite_table th {
    padding: 2px 2px;
}
tr.satellite_row:hover td, tr.satellite_row:hover th {
    background-color: white !important;
}
.satellite_row {
    display: none;
}
</style>

<h1 class="docs"><?php echo Configure::read('Website.name'); ?> API Documentation</h1>
<p><span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> is a publically accessible API that allows users or programs to retrieve TLE's for specified sources and satellites in a variety of formats. <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> was developed by <a href="http://exploration.engin.umich.edu/blog/" target="_blank" class="link">The Michigan Exploration Laboratory</a> for use as an internal TLE management system.</p>

<!-- Using the API -->
<h2 class="docs"><a name="using_api"><a href="#using_api" class="doc_link">1.0 Using the API</a></a></h2>
<p>The <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> API uses a standard <a href="http://en.wikipedia.org/wiki/Representational_state_transfer" target="_blank" class="link">HTTP RESTful API scheme</a>. <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> works by aggregating several TLE sources (such as <a href="http://celestrak.com/" target="_blank" class="link">CelesTrak's</a> TLE collections) into a timestamped index of satellite TLE's accessible by an API. This API allows you to retrieve the TLE information for any collection of TLE sources or available satellites. Currently, no authentication is required to use the <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> API and there are no usage limits, however this is subject to change. If you are unfamiliar with using REST APIs, <a href="http://rest.elkstein.org/" target="_blank" class="link">this</a> is a good resource.</p>

<h3 class="docs"><a name="making_request"><a href="#making_request" class="doc_link">1.1 Making An API Request</a></a></h3>
<p>The API is a simple collection of URL's that respond to GET requests. Such resources are commonly accessed using utilities such as <a href="http://www.gnu.org/software/wget/" target="_blank" class="link">wget</a> or <a href="http://curl.haxx.se/" target="_blank" class="link">cURL</a>. All API endpoints have the same basic format:</p>
<pre>
<?php echo Router::url('/', true); ?>api/[resource type]/[collection of resources].[format]
</pre>
<p>The different types of resources are explained in detail in the following sections.</p>
<p>The resources specified in the list of resources passed to the API, indicated above by <span style="font-style: italic;">[collection of resources]</span> <strong>must</strong> be <a href="http://us2.php.net/manual/en/function.rawurlencode.php" target="_blank" class="link">URL encoded</a> before being joined together and submitted. This is required so that <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> can successfully process the request. It is very important to URL encode each item separately before joining them with the underscore ("_") character. In addition, spaces must be encoded as "&20" instead of "+" (use the PHP function rawurlencode instead of urlencode).</p>
<p>Currently, the results of an API request are cached for 30 minutes for performance reasons.</p>

<h3 class="docs"><a name="making_request"><a href="#making_request" class="doc_link">1.2 Response Formats</a></a></h3>
<p>Currently, <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> can respond in four different formats: XML, JSON, JSONP, and raw TLE format. To request a result in JSON/JSONP or XML just append .json or .xml, respectively, to the API endpoint URL. To request a raw TLE file, simple remove the <span style="font-style: italic;">[format]</span> postfix all together. Note that to generate a JSONP response (which allows for the cross-domain resource loading commonly used in AJAX scripts), you must specify a callback. For example:</p>
<pre>
<?php echo Router::url('/', true); ?>api/sources/CUBESAT.json?callback=yourcallback
</pre>

<h3 class="docs"><a name="parameters"><a href="#parameters" class="doc_link">1.3 Shared Resource Parameters</a></a></h3>
<p>There are several parameters that you can append to your API request that allow you to customize the results returned.</p>
<table class="table table-condensed">
	<thead>
		<tr>
			<th width="10%">Parameter</th>
			<th width="10%">Type</th>
			<th width="50%">Effect</th>
			<th width="30%">Example</th>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td width="10%">callback</td>
			<td width="10%">String</td>
			<td width="50%">Adding the callback parameter to your request (when the format is set to .json) returns the results in JSONP format. This use commonly used when making requests from an AJAX or otherwise client-side script.</td>
			<td width="30%"><span style="font-style: italic;">/sources/CUBESAT.json?callback=yourcallback</span></td>
		</tr>
		<tr>
			<td width="10%">timestamp</td>
			<td width="10%">Integer (UNIX Timestamp)</td>
			<td width="50%">The timestamp parameter allows you to supply a <a href="http://www.unixtimestamp.com/index.php" target="_blank" class="link">UNIX timestamp</a> with your request. The result will consist of the TLE's that <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> has that are closest to the timestamp you provided. Without a timestamp, <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> returns the most recent TLE for each satellite included in the response. Note this parameter is relative to server time (EST).</td>
			<td width="30%"><span style="font-style: italic;">/satellites/RAX-2.xml?timestamp=1339736400</span></td>
		</tr>
		<!--<tr>
			<td width="10%">limit</td>
			<td width="10%">Integer</td>
			<td width="50%">This parameter allows you to limit the number of satellite TLEs returned in any given request. Note that this always limits the number of TLEs, not the number of sources. So the query to the right, for example, would return the first 3 satellites for both sources (GPS and CUBESAT).</td>
			<td width="30%"><span style="font-style: italic;">/sources/GPS+CUBESAT.xml?limit=3</span></td>
		</tr>
		<tr>
			<td width="10%">offset</td>
			<td width="10%">Integer</td>
			<td width="50%">The offset parameter allows you to specify how to offset the response. This must be used with the limit parameter defined above. For example, you may limit the results to 3 and then use offset to retrieve the next group of 3 TLEs.</td>
			<td width="30%"><span style="font-style: italic;">/sources/GPS+CUBESAT.xml?limit=3&offset=1</span></td>
		</tr>-->
	</tbody>
</table>
<p>These shared parameters can be used on any resource type.</p>

<h3 class="docs"><a name="resource_sources"><a href="#resource_sources" class="doc_link">1.4 Sources Resources: /api/sources/[sources].[format]</a></a></h3>
<p>The <span style="font-style: italic;">sources</span> resource allows you to retrieve the TLE's for the specified sources. Any number of sources can be requested by joining their identifiers together with underscores. For example, to retrieve the most recent TLEs in JSON for both the GPS and CUBESAT sources this request would be used:</p>
<pre>
<?php echo Router::url('/', true); ?>api/sources/CUBESAT_GPS.json
</pre>
<p>As stated in the <a href="#making_request" class="doc_link">Making An API Request</a> section, all source names must be URL encoded before being joined with the underscore.</p>
<p>Instead of explaining every field returned in the response, portions of the response (clipped parts of the response are indicated by a "...") from the example request above are displayed below in every available format.</p>
<ul class="nav nav-tabs" id="sources_examples">
	<li class="active"><a href="#sources_json">JSON</a></li>
	<li><a href="#sources_xml">XML</a></li>
	<li><a href="#sources_raw">Raw TLEs</a></li>
</ul>
<div class="tab-content">
	<div class="tab-pane active" id="sources_json">
<pre class="prettyprint pre-scrollable">
{
  "sources":{
    "GPS":{
      "satellites":{
        "GPS BIIA-10 (PRN 32)":{
          "name":"GPS BIIA-10 (PRN 32)",
          "satellite_number":"20959",
          "classification":"U",
          "launch_year":"90",
          "launch_number":"103",
          "launch_piece":"A",
          "epoch_year":"12",
          "epoch":"208.35547222",
          "ftd_mm_d2":".00000064",
          "std_mm_d6":"00000-0",
          "bstar_drag":"10000-3",
          "element_number":"651",
          "checksum_l1":"0",
          "inclination":"54.5008",
          "right_ascension":"237.0787",
          "eccentricity":"0119318",
          "perigee":"327.5321",
          "mean_anomaly":"31.8169",
          "mean_motion":"2.00568072",
          "revs":"15867",
          "checksum_l2":"4",
          "raw_l1":"1 20959U 90103A   12208.35547222  .00000064  00000-0  10000-3 0  6510",
          "raw_l2":"2 20959  54.5008 237.0787 0119318 327.5321  31.8169  2.00568072158674"
        },
        ...
      },
      "status":{
        "url":"http:\/\/www.celestrak.com\/NORAD\/elements\/gps-ops.txt",
        "description":"GPS Satellites.",
        "updated":1343494980,
        "satellites_fetched":31
      }
    },
    "CUBESAT":{
      "satellites":{
        "DTUSAT":{
          "name":"DTUSAT",
          "satellite_number":"27842",
          "classification":"U",
          "launch_year":"3",
          "launch_number":"31",
          "launch_piece":"C",
          "epoch_year":"12",
          "epoch":"209.55574681",
          "ftd_mm_d2":".00000096",
          "std_mm_d6":"00000-0",
          "bstar_drag":"64011-4",
          "element_number":"454",
          "checksum_l1":"2",
          "inclination":"98.6957",
          "right_ascension":"217.7612",
          "eccentricity":"0010295",
          "perigee":"81.8434",
          "mean_anomaly":"278.3917",
          "mean_motion":"14.21308551",
          "revs":"47074",
          "checksum_l2":"9",
          "raw_l1":"1 27842U 03031C   12209.55574681  .00000096  00000-0  64011-4 0  4542",
          "raw_l2":"2 27842  98.6957 217.7612 0010295  81.8434 278.3917 14.21308551470749"
        },
        ...
      },
      "status":{
        "url":"http:\/\/celestrak.com\/NORAD\/elements\/cubesat.txt",
        "description":"CUBESAT satellites",
        "updated":1343494981,
        "satellites_fetched":39
      }
    }
  },
  "status":{
    "status":"okay",
    "message":"At least one of the specified sources was loaded.",
    "timestamp":1343498683,
    "sources_fetched":2
  }
}
</pre>
	</div>
	<div class="tab-pane" id="sources_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0"?&gt;
&lt;api_sources&gt;
  &lt;status&gt;
    &lt;status&gt;okay&lt;/status&gt;
    &lt;message&gt;At least one of the specified sources was loaded.&lt;/message&gt;
    &lt;timestamp&gt;1343499500&lt;/timestamp&gt;
    &lt;sources_fetched&gt;2&lt;/sources_fetched&gt;
  &lt;/status&gt;
  &lt;sources&gt;
    &lt;source name="GPS"&gt;
      &lt;status&gt;
        &lt;url&gt;http://www.celestrak.com/NORAD/elements/gps-ops.txt&lt;/url&gt;
        &lt;description&gt;GPS Satellites.&lt;/description&gt;
        &lt;updated&gt;1343494980&lt;/updated&gt;
        &lt;satellites_fetched&gt;31&lt;/satellites_fetched&gt;
      &lt;/status&gt;
      &lt;satellites&gt;
        &lt;satellite name="GPS BIIA-10 (PRN 32)"&gt;
          &lt;name&gt;GPS BIIA-10 (PRN 32)&lt;/name&gt;
          &lt;satellite_number&gt;20959&lt;/satellite_number&gt;
          &lt;classification&gt;U&lt;/classification&gt;
          &lt;launch_year&gt;90&lt;/launch_year&gt;
          &lt;launch_number&gt;103&lt;/launch_number&gt;
          &lt;launch_piece&gt;A&lt;/launch_piece&gt;
          &lt;epoch_year&gt;12&lt;/epoch_year&gt;
          &lt;epoch&gt;208.35547222&lt;/epoch&gt;
          &lt;ftd_mm_d2&gt;.00000064&lt;/ftd_mm_d2&gt;
          &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
          &lt;bstar_drag&gt;10000-3&lt;/bstar_drag&gt;
          &lt;element_number&gt;651&lt;/element_number&gt;
          &lt;checksum_l1&gt;0&lt;/checksum_l1&gt;
          &lt;inclination&gt;54.5008&lt;/inclination&gt;
          &lt;right_ascension&gt;237.0787&lt;/right_ascension&gt;
          &lt;eccentricity&gt;0119318&lt;/eccentricity&gt;
          &lt;perigee&gt;327.5321&lt;/perigee&gt;
          &lt;mean_anomaly&gt;31.8169&lt;/mean_anomaly&gt;
          &lt;mean_motion&gt;2.00568072&lt;/mean_motion&gt;
          &lt;revs&gt;15867&lt;/revs&gt;
          &lt;checksum_l2&gt;4&lt;/checksum_l2&gt;
          &lt;raw_l1&gt;1 20959U 90103A   12208.35547222  .00000064  00000-0  10000-3 0  6510&lt;/raw_l1&gt;
          &lt;raw_l2&gt;2 20959  54.5008 237.0787 0119318 327.5321  31.8169  2.00568072158674&lt;/raw_l2&gt;
        &lt;/satellite&gt;
        ...
      &lt;/satellites&gt;
    &lt;/source&gt;
    &lt;source name="CUBESAT"&gt;
      &lt;status&gt;
        &lt;url&gt;http://celestrak.com/NORAD/elements/cubesat.txt&lt;/url&gt;
        &lt;description&gt;CUBESAT satellites&lt;/description&gt;
        &lt;updated&gt;1343494981&lt;/updated&gt;
        &lt;satellites_fetched&gt;39&lt;/satellites_fetched&gt;
      &lt;/status&gt;
      &lt;satellites&gt;
        &lt;satellite name="DTUSAT"&gt;
          &lt;name&gt;DTUSAT&lt;/name&gt;
          &lt;satellite_number&gt;27842&lt;/satellite_number&gt;
          &lt;classification&gt;U&lt;/classification&gt;
          &lt;launch_year&gt;3&lt;/launch_year&gt;
          &lt;launch_number&gt;31&lt;/launch_number&gt;
          &lt;launch_piece&gt;C&lt;/launch_piece&gt;
          &lt;epoch_year&gt;12&lt;/epoch_year&gt;
          &lt;epoch&gt;209.55574681&lt;/epoch&gt;
          &lt;ftd_mm_d2&gt;.00000096&lt;/ftd_mm_d2&gt;
          &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
          &lt;bstar_drag&gt;64011-4&lt;/bstar_drag&gt;
          &lt;element_number&gt;454&lt;/element_number&gt;
          &lt;checksum_l1&gt;2&lt;/checksum_l1&gt;
          &lt;inclination&gt;98.6957&lt;/inclination&gt;
          &lt;right_ascension&gt;217.7612&lt;/right_ascension&gt;
          &lt;eccentricity&gt;0010295&lt;/eccentricity&gt;
          &lt;perigee&gt;81.8434&lt;/perigee&gt;
          &lt;mean_anomaly&gt;278.3917&lt;/mean_anomaly&gt;
          &lt;mean_motion&gt;14.21308551&lt;/mean_motion&gt;
          &lt;revs&gt;47074&lt;/revs&gt;
          &lt;checksum_l2&gt;9&lt;/checksum_l2&gt;
          &lt;raw_l1&gt;1 27842U 03031C   12209.55574681  .00000096  00000-0  64011-4 0  4542&lt;/raw_l1&gt;
          &lt;raw_l2&gt;2 27842  98.6957 217.7612 0010295  81.8434 278.3917 14.21308551470749&lt;/raw_l2&gt;
        &lt;/satellite&gt;
        ...
      &lt;/satellites&gt;
    &lt;/source&gt;
  &lt;/sources&gt;
&lt;/api_sources&gt;
</pre>
	</div>
	<div class="tab-pane" id="sources_raw">
<pre class="pre-scrollable">
GPS BIIA-10 (PRN 32)
1 20959U 90103A   12208.35547222  .00000064  00000-0  10000-3 0  6510
2 20959  54.5008 237.0787 0119318 327.5321  31.8169  2.00568072158674
GPS BIIA-14 (PRN 26)
1 22014U 92039A   12208.15104310 -.00000053  00000-0  10000-3 0  4538
2 22014  56.2990 297.0869 0206300  68.9161 293.3261  2.00565047140409
...
DTUSAT
1 27842U 03031C   12209.55574681  .00000096  00000-0  64011-4 0  4542
2 27842  98.6957 217.7612 0010295  81.8434 278.3917 14.21308551470749
CUTE-1 (CO-55)
1 27844U 03031E   12209.05588583  .00000097  00000-0  64826-4 0  4269
2 27844  98.7023 216.8548 0010504 104.6696 255.5651 14.21046409470553
...
</pre>
	</div>
</div>
<p>If any of the sources can't be located, they will simply be omitted from the response. However, if none of the sources are valid an error will be generated.</p>

<h3 class="docs"><a name="resource_satellites"><a href="#resource_satellites" class="doc_link">1.5 Satellites Resources: /api/satellites/[satellites].[format]</a></a></h3>
<p>The <span style="font-style: italic;">satellites</span> resource allows you to retrieve the TLE's for the specified satellites. Any number of satellites can be requested by joining their identifiers together with underscores. For example, to retrieve the most recent TLEs in JSON for the "RAX-2" and "CUTE-1.7+APD II (CO-65)" satellites this request would be used:</p>
<pre>
<?php echo Router::url('/', true); ?>api/satellites/RAX-2_CUTE-1.7%2BAPD%20II%20(CO-65).json
</pre>
<p>As stated in the <a href="#making_request" class="link">Making An API Request</a> section, all satellite names must be URL encoded (like the "CUTE-1.7+APD II (CO-65)" satellite is in the example request above) before being joined with the underscore.</p>
<p>Instead of explaining every field returned in the response, portions of the response (clipped parts of the response are indicated by a "...") from the example request above are displayed below in every available format.</p>
<ul class="nav nav-tabs" id="satellites_examples">
    <li class="active"><a href="#satellites_json">JSON</a></li>
    <li><a href="#satellites_xml">XML</a></li>
    <li><a href="#satellites_raw">Raw TLEs</a></li>
</ul>
<div class="tab-content">
	<div class="tab-pane active" id="satellites_json">
<pre class="prettyprint pre-scrollable">
{
  "satellites":{
    "CUTE-1.7+APD II (CO-65)":{
      "status":{
        "updated":1343494981
      },
      "tle":{
        "name":"CUTE-1.7+APD II (CO-65)",
        "satellite_number":"32785",
        "classification":"U",
        "launch_year":"8",
        "launch_number":"21",
        "launch_piece":"C",
        "epoch_year":"12",
        "epoch":"210.06412002",
        "ftd_mm_d2":".00000537",
        "std_mm_d6":"00000-0",
        "bstar_drag":"72400-4",
        "element_number":"371",
        "checksum_l1":"3",
        "inclination":"97.7968",
        "right_ascension":"270.1455",
        "eccentricity":"0013397",
        "perigee":"290.2902",
        "mean_anomaly":"69.6887",
        "mean_motion":"14.83071185",
        "revs":"22982",
        "checksum_l2":"9",
        "raw_l1":"1 32785U 08021C   12210.06412002  .00000537  00000-0  72400-4 0  3713",
        "raw_l2":"2 32785  97.7968 270.1455 0013397 290.2902  69.6887 14.83071185229829"
      }
    },
    "RAX-2":{
      "status":{
        "updated":1343494981
      },
      "tle":{
        "name":"RAX-2",
        "satellite_number":"37853",
        "classification":"U",
        "launch_year":"11",
        "launch_number":"61",
        "launch_piece":"D",
        "epoch_year":"12",
        "epoch":"210.14980750",
        "ftd_mm_d2":".00001563",
        "std_mm_d6":"00000-0",
        "bstar_drag":"13310-3",
        "element_number":"241",
        "checksum_l1":"1",
        "inclination":"101.7115",
        "right_ascension":"268.9927",
        "eccentricity":"0246359",
        "perigee":"234.1411",
        "mean_anomaly":"123.6687",
        "mean_motion":"14.80240914",
        "revs":"4045",
        "checksum_l2":"1",
        "raw_l1":"1 37853U 11061D   12210.14980750  .00001563  00000-0  13310-3 0  2411",
        "raw_l2":"2 37853 101.7115 268.9927 0246359 234.1411 123.6687 14.80240914 40451"
      }
    }
  },
  "status":{
    "status":"okay",
    "message":"At least one of the specified satellites was loaded.",
    "timestamp":1343500821,
    "satellites_fetched":2
  }
}
</pre>
	</div>
	<div class="tab-pane" id="satellites_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0"?&gt;
&lt;api_satellites&gt;
  &lt;status&gt;
    &lt;status&gt;okay&lt;/status&gt;
    &lt;message&gt;At least one of the specified satellites was loaded.&lt;/message&gt;
    &lt;timestamp&gt;1343501121&lt;/timestamp&gt;
    &lt;satellites_fetched&gt;2&lt;/satellites_fetched&gt;
  &lt;/status&gt;
  &lt;satellites&gt;
    &lt;satellite name='CUTE-1.7+APD II (CO-65)'&gt;
      &lt;status&gt;
        &lt;updated&gt;1343494981&lt;/updated&gt;
      &lt;/status&gt;
      &lt;tle&gt;
        &lt;name&gt;CUTE-1.7+APD II (CO-65)&lt;/name&gt;
        &lt;satellite_number&gt;32785&lt;/satellite_number&gt;
        &lt;classification&gt;U&lt;/classification&gt;
        &lt;launch_year&gt;8&lt;/launch_year&gt;
        &lt;launch_number&gt;21&lt;/launch_number&gt;
        &lt;launch_piece&gt;C&lt;/launch_piece&gt;
        &lt;epoch_year&gt;12&lt;/epoch_year&gt;
        &lt;epoch&gt;210.06412002&lt;/epoch&gt;
        &lt;ftd_mm_d2&gt;.00000537&lt;/ftd_mm_d2&gt;
        &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
        &lt;bstar_drag&gt;72400-4&lt;/bstar_drag&gt;
        &lt;element_number&gt;371&lt;/element_number&gt;
        &lt;checksum_l1&gt;3&lt;/checksum_l1&gt;
        &lt;inclination&gt;97.7968&lt;/inclination&gt;
        &lt;right_ascension&gt;270.1455&lt;/right_ascension&gt;
        &lt;eccentricity&gt;0013397&lt;/eccentricity&gt;
        &lt;perigee&gt;290.2902&lt;/perigee&gt;
        &lt;mean_anomaly&gt;69.6887&lt;/mean_anomaly&gt;
        &lt;mean_motion&gt;14.83071185&lt;/mean_motion&gt;
        &lt;revs&gt;22982&lt;/revs&gt;
        &lt;checksum_l2&gt;9&lt;/checksum_l2&gt;
        &lt;raw_l1&gt;1 32785U 08021C   12210.06412002  .00000537  00000-0  72400-4 0  3713&lt;/raw_l1&gt;
        &lt;raw_l2&gt;2 32785  97.7968 270.1455 0013397 290.2902  69.6887 14.83071185229829&lt;/raw_l2&gt;
      &lt;/tle&gt;
    &lt;/satellite&gt;
    &lt;satellite name='RAX-2'&gt;
      &lt;status&gt;
        &lt;updated&gt;1343494981&lt;/updated&gt;
      &lt;/status&gt;
      &lt;tle&gt;
        &lt;name&gt;RAX-2&lt;/name&gt;
        &lt;satellite_number&gt;37853&lt;/satellite_number&gt;
        &lt;classification&gt;U&lt;/classification&gt;
        &lt;launch_year&gt;11&lt;/launch_year&gt;
        &lt;launch_number&gt;61&lt;/launch_number&gt;
        &lt;launch_piece&gt;D&lt;/launch_piece&gt;
        &lt;epoch_year&gt;12&lt;/epoch_year&gt;
        &lt;epoch&gt;210.14980750&lt;/epoch&gt;
        &lt;ftd_mm_d2&gt;.00001563&lt;/ftd_mm_d2&gt;
        &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
        &lt;bstar_drag&gt;13310-3&lt;/bstar_drag&gt;
        &lt;element_number&gt;241&lt;/element_number&gt;
        &lt;checksum_l1&gt;1&lt;/checksum_l1&gt;
        &lt;inclination&gt;101.7115&lt;/inclination&gt;
        &lt;right_ascension&gt;268.9927&lt;/right_ascension&gt;
        &lt;eccentricity&gt;0246359&lt;/eccentricity&gt;
        &lt;perigee&gt;234.1411&lt;/perigee&gt;
        &lt;mean_anomaly&gt;123.6687&lt;/mean_anomaly&gt;
        &lt;mean_motion&gt;14.80240914&lt;/mean_motion&gt;
        &lt;revs&gt;4045&lt;/revs&gt;
        &lt;checksum_l2&gt;1&lt;/checksum_l2&gt;
        &lt;raw_l1&gt;1 37853U 11061D   12210.14980750  .00001563  00000-0  13310-3 0  2411&lt;/raw_l1&gt;
        &lt;raw_l2&gt;2 37853 101.7115 268.9927 0246359 234.1411 123.6687 14.80240914 40451&lt;/raw_l2&gt;
      &lt;/tle&gt;
    &lt;/satellite&gt;
  &lt;/satellites&gt;
&lt;/api_satellites&gt;
</pre>
	</div>
	<div class="tab-pane" id="satellites_raw">
<pre class="pre-scrollable">
CUTE-1.7+APD II (CO-65)
1 32785U 08021C   12210.06412002  .00000537  00000-0  72400-4 0  3713
2 32785  97.7968 270.1455 0013397 290.2902  69.6887 14.83071185229829
RAX-2
1 37853U 11061D   12210.14980750  .00001563  00000-0  13310-3 0  2411
2 37853 101.7115 268.9927 0246359 234.1411 123.6687 14.80240914 40451
</pre>
	</div>
</div>
<p>If any of the satellites can't be located, they will simply be omitted from the response. However, if none of the satellites are valid an error will be generated.</p>

<h3 class="docs"><a name="errors"><a href="#errors" class="doc_link">1.6 API Errors</a></a></h3>
<p>The top level status element for both the sources and satellites resources will always be returned, even in the event of an error (although this will not occur with the occasional server error). For example, the result of requesting a single non-existent satellite would be:</p>
<ul class="nav nav-tabs" id="error_examples">
	<li class="active"><a href="#error_json">JSON</a></li>
	<li><a href="#error_xml">XML</a></li>
	<li><a href="#error_raw">Raw TLEs</a></li>
</ul>
<div class="tab-content">
	<div class="tab-pane active" id="error_json">
<pre class="prettyprint pre-scrollable">
{
  "status":{
    "status":"error",
    "message":"None of the provided satellites could be located or have been updated recently.",
    "timestamp":1343502007,
    "satellites_fetched":0
  }
}
</pre>
	</div>
	<div class="tab-pane" id="error_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0"?&gt;
&lt;api_satellites&gt;
  &lt;status&gt;
    &lt;status&gt;error&lt;/status&gt;
    &lt;message&gt;None of the provided satellites could be located or have been updated recently.&lt;/message&gt;
    &lt;timestamp&gt;1343502042&lt;/timestamp&gt;
    &lt;satellites_fetched&gt;0&lt;/satellites_fetched&gt;
  &lt;/status&gt;
&lt;/api_satellites&gt;
</pre>
	</div>
	<div class="tab-pane" id="error_raw">
<pre class="pre-scrollable">
[ERROR] No TLEs could be found. 
</pre>
	</div>
</div>

<!-- Available Satellites & Sources -->
<h2 class="docs"><a name="available_satellites"><a href="#available_satellites" class="doc_link">2.0 Available Sources and Satellites</a></a></h2>
<p>Below, lists of the available sources and their satellites along with their URL encoding are displayed for your convenience.</p>
<?php if (!empty($sources)): ?>
    <table class="table">
	<thead>
	    <tr>
		<th width="15%">Source Name</th>
		<th width="15%">Encoded Name</th>
		<th width="20%">Last Updated</th>
		<th width="15%">Description</th>
		<th width="35%">URL</th>
	    </tr>
	</thead>
    <tbody>
	<?php foreach($sources as $source): ?>
	    <tr id="source_<?php echo $source['Source']['id']; ?>">
		<td width="15%"><?php echo $source['Source']['name']; ?> <a class="expand_link" rel="<?php echo $source['Source']['id']; ?>">[+ Satellites]</a></td>
		<td width="15%"><?php echo rawurlencode($source['Source']['name']); ?></td>
		<td width="20%">
		    <?php if (isset($source['Update'][0]['created_on'])): ?>
			<?php echo date('m/d/Y G:i:s T', strtotime($source['Update'][0]['created_on'])); ?>
		    <?php else: ?>
			<span style="font-style: italic;">Not Updated Yet</span>
		    <?php endif; ?>
		</td>
		<td width="15%"><?php echo $source['Source']['description']; ?></td>
		<td width="35%"><a href="<?php echo $source['Source']['url']; ?>" target="_blank" class="link"><?php echo $source['Source']['url']; ?></a></td>
	    </tr>
	    <tr id="satellites_<?php echo $source['Source']['id']; ?>" class="satellite_row">
		<td colspan="1"></td>
		<td colspan="4">
		    <?php if (!empty($source['Update'][0]['Tle'])): ?>
			<table style="width: 50%;" class="table table-condensed satellite_table">
			    <thead>
				<th width="50%">Satellite Name</th>
				<th width="50%">Encoded Satellite Name</th>
			    </thead>
			    <tbody>
				<?php foreach($source['Update'][0]['Tle'] as $tle): ?>
				    <tr>
					<td><?php echo $tle['name']; ?></td>
					<td><?php echo rawurlencode($tle['name']); ?></td>
				    </tr>
				<?php endforeach; ?>
			    </tbody>
			</table>
		    <?php else: ?>
			<span style="font-style: italic;">No Satellites Loaded</span>
		    <?php endif; ?>
		</td>
	    </tr>
	<?php endforeach; ?>
    </tbody>
    </table>
<?php else: ?>
	<span style="font-style: italic;">No sources are currently available for use.</span>
<?php endif; ?>
