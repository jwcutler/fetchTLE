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
  $('#positions_examples a').click(function (e) {
    e.preventDefault();
    $(this).tab('show');
  })
  $('#passes_examples a').click(function (e) {
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

<h1 class="titles"><?php echo Configure::read('Website.name'); ?> API Documentation</h1>
<p><span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> is a publically accessible API that allows users or programs to retrieve TLE's for specified sources and satellites in a variety of formats. <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> was developed by <a href="http://exploration.engin.umich.edu/blog/" target="_blank" class="link">The Michigan Exploration Laboratory</a> for use as an internal TLE management system.</p>

<!-- Using the API -->
<h2 class="titles"><a name="using_api"><a href="#using_api" class="doc_link">1.0 Using the API</a></a></h2>
<p>The <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> API uses a standard <a href="http://en.wikipedia.org/wiki/Representational_state_transfer" target="_blank" class="link">HTTP RESTful API scheme</a>. <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> works by aggregating several TLE sources (such as <a href="http://celestrak.com/" target="_blank" class="link">CelesTrak's</a> TLE collections) into a timestamped index of satellite TLE's accessible by an API. This API allows you to retrieve the TLE information for any collection of TLE sources or available satellites. In addition, this API allows you calculate the position of a satellite at given intervals within an arbitrary range of time. Currently, no authentication is required to use the <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> API and there are no usage limits, however this is subject to change. If you are unfamiliar with using REST APIs, <a href="http://rest.elkstein.org/" target="_blank" class="link">this</a> is a good resource.</p>

<h3 class="titles"><a name="making_request"><a href="#making_request" class="doc_link">1.1 Making An API Request</a></a></h3>
<p>The API is a simple collection of URL's that respond to GET requests. Such resources are commonly accessed using utilities such as <a href="http://www.gnu.org/software/wget/" target="_blank" class="link">wget</a> or <a href="http://curl.haxx.se/" target="_blank" class="link">cURL</a>. All API endpoints have the same basic format:</p>
<pre>
<?php echo Router::url('/', true); ?>api/[resource type]/[collection of resources].[format]
</pre>
<p>The different types of resources are explained in detail in the following sections.</p>
<p>The resources specified in the list of resources passed to the API, indicated above by <span style="font-style: italic;">[collection of resources]</span> <strong>must</strong> be <a href="http://us2.php.net/manual/en/function.rawurlencode.php" target="_blank" class="link">URL encoded</a> before being joined together and submitted. This is required so that <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> can successfully process the request. It is very important to URL encode each item separately before joining them with the underscore ("_") character. In addition, spaces must be encoded as "&20" instead of "+" (use the PHP function rawurlencode instead of urlencode).</p>
<p>Currently, the results of an API request are cached for 30 minutes for performance reasons.</p>

<h3 class="titles"><a name="making_request"><a href="#making_request" class="doc_link">1.2 Response Formats</a></a></h3>
<p>Currently, <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> can respond in four different formats: XML, JSON, JSONP, and raw TEXT format. To request a result in JSON/JSONP or XML just append .json or .xml, respectively, to the API endpoint URL. To request the raw resource dump, simple remove the <span style="font-style: italic;">[format]</span> postfix all together. Note that to generate a JSONP response (which allows for the cross-domain resource loading commonly used in AJAX scripts), you must specify a callback. For example:</p>
<pre>
<?php echo Router::url('/', true); ?>api/sources/CUBESAT.json?callback=yourcallback
</pre>

<h3 class="titles"><a name="parameters"><a href="#parameters" class="doc_link">1.3 Request Parameters</a></a></h3>
<p>There are several parameters that you can append to your API request that allow you to customize the results returned.</p>
<table class="table table-condensed table-hover">
  <thead>
    <tr>
      <th width="10%">Parameter</th>
      <th width="10%">Type</th>
      <th width="10%">Works With</th>
      <th width="40%">Effect</th>
      <th width="30%">Example</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td colspan='1'>callback</td>
      <td colspan='1'>String</td>
      <td colspan='1'>satellites<br />sources<br />positions<br />passes</td>
      <td colspan='1'>Adding the callback parameter to your request (when the format is set to .json) returns the results in JSONP format. This use commonly used when making requests from an AJAX or otherwise client-side script.</td>
      <td colspan='1'><span style="font-style: italic;">/sources/CUBESAT.json?callback=yourcallback</span></td>
    </tr>
    <tr>
      <td colspan='1'>timestamp</td>
      <td colspan='1'>Integer (UNIX Timestamp)</td>
      <td colspan='1'>satellites<br />sources<br />passes</td>
      <td colspan='1'>The timestamp parameter allows you to supply a <a href="http://www.unixtimestamp.com/index.php" target="_blank" class="link">UNIX timestamp</a> with your request. The result will consist of (or use for calculations) the TLEs that are closest to the timestamp you provided. Without a timestamp, <span style="font-style: italic;"><?php echo Configure::read('Website.name'); ?></span> uses the most recent TLE for each satellite included in the response. When used with the passes API, this parameter specifies the start date that you would like to use when calculating satellite passes.</td>
      <td colspan='1'><span style="font-style: italic;">/satellites/RAX-2.xml?timestamp=1339736400</span></td>
    </tr>
    <tr>
      <td colspan='1'>start</td>
      <td colspan='1'>Integer (UNIX Timestamp)</td>
      <td colspan='1'>satellites<br />positions</td>
      <td colspan='1'>This parameter, when used with the positions API, allows you to specify the beginning of the range of times that you would like to calculate satellite positions during. This timestamp will be used to select the TLE for the specified satellite(s) that is closest to the beginning of the range. That TLE will then be used to calculate the satellite positions during the interval. If no start timestamp is specified, the time that the request was submitted will be used instead. When used with the satellites API, the start parameter specifies the beginning (inclusive) timestamp for a set of TLEs. Including the start parameter will cause the response to be paginated (see the count and page parameters below). In addition, the start parameter will take precedence over the timestamp parameter. It is a UTC <a href="http://www.unixtimestamp.com/index.php" target="_blank" class="link">UNIX timestamp</a>.</td>
      <td colspan='1'><span style="font-style: italic;">/positions/RAX-2.xml?start=1339736400</span></td>
    </tr>
    <tr>
      <td colspan='1'>end</td>
      <td colspan='1'>Integer (UNIX Timestamp)</td>
      <td colspan='1'>positions</td>
      <td colspan='1'>This parameter, when used with the positions API, allows you to specify the end of the range of times that you would like to calculate satellite positions during. If no end timestamp is specified, it will automatically be set to be 24 hours ahead of the start timestamp. It is a UTC <a href="http://www.unixtimestamp.com/index.php" target="_blank" class="link">UNIX timestamp</a>.</td>
      <td colspan='1'><span style="font-style: italic;">/positions/RAX-2.xml?start=1339736400&end=1339822800</span></td>
    </tr>
    <tr>
      <td colspan='1'>count</td>
      <td colspan='1'>Integer</td>
      <td colspan='1'>satellites</td>
      <td colspan='1'>When used with start, the count parameter specifies the number of TLEs returned per page, per satellite, in the API response. Defaults to <?php echo Configure::read('fetchTLE.tleCount') ?> and has a max value of <?php echo Configure::read('fetchTLE.tleCountMax') ?>.</td>
      <td colspan='1'><span style="font-style: italic;">/satellites/RAX-2.xml?start=1339736400&count=5</span></td>
    </tr>
    <tr>
      <td colspan='1'>page</td>
      <td colspan='1'>Integer</td>
      <td colspan='1'>satellites</td>
      <td colspan='1'>When used with the start and count parameters, page specifies the current page of the result set. Note that result sets are not persisted between requests. This means that if an update occurs while you are iterating over the pages, some requests may contain duplicate TLEs.</td>
      <td colspan='1'><span style="font-style: italic;">/satellites/RAX-2.xml?start=1339736400&count=5&page=2</span></td>
    </tr>
    <tr>
      <td colspan='1'>resolution</td>
      <td colspan='1'>Integer (seconds)</td>
      <td colspan='1'>positions</td>
      <td colspan='1'>This parameter specifies how often you would like satellite position calculations to occur during the specified range. For example, if set to '60' the satellite positions calculated will be 60 seconds apart. Defaults to 60 if not specified.</td>
      <td colspan='1'><span style="font-style: italic;">/positions/RAX-2.xml?resolution=10</span></td>
    </tr>
    <tr>
      <td colspan='1'>pass_count</td>
      <td colspan='1'>Integer</td>
      <td colspan='1'>passes</td>
      <td colspan='1'>This parameter allows you to specify how many acceptable passes (i.e. passes that meet the minimum elevation constraints) you wanted included in the result. If the supplied value for this parameter is too large (either exceeds the set limit or if the propagator can't calculate the requested number of specified acceptable passes in one go), a default value will be used instead. Once <span style="font-style: italic;">pass_count</span> acceptable passes are calculated, no further passes will be included in the response. Note that this value is used to limit only the acceptable passes. That is to say, if the 'show_all_passes' parameter is set to true unacceptable passes won't be considered when checking this parameter.</td>
      <td colspan='1'><span style="font-style: italic;">/passes/RAX-2.json?pass_count=5</span></td>
    </tr>
    <tr>
      <td colspan='1'>show_all_passes</td>
      <td colspan='1'>Boolean</td>
      <td colspan='1'>passes</td>
      <td colspan='1'>This parameter allows you to specify whether or not you would like unacceptable passes (i.e. passes that don't meet the elevation requirements) to be included in the response. Defaults to 'true' if not specified.</td>
      <td colspan='1'><span style="font-style: italic;">/passes/RAX-2.json?show_all_passes=true</span></td>
    </tr>
    <tr>
      <td colspan='1'>ground_stations</td>
      <td colspan='1'>String</td>
      <td colspan='1'>passes</td>
      <td colspan='1'>The 'ground_stations' parameter allows you to specify what ground stations you would like to consider when calculating pass times for a specified satellite. It is a string consisting of URL encoded ground station names joined with underscores ("_"). If no ground stations are specified, a collection of default ground stations will be used instead. For a list of available ground stations, see the list in section <a href="#available_satellites" class="link">2.0</a> of this document.</td>
      <td colspan='1'><span style="font-style: italic;">/passes/RAX-2.json?ground_stations=FXB_SRI</span></td>
    </tr>
    <tr>
      <td colspan='1'>min_elevations</td>
      <td colspan='1'>String</td>
      <td colspan='1'>passes</td>
      <td colspan='1'>This parameter, when used with the 'ground_stations' parameter, allows you to specify the minimum elevation to use for each ground station. It is comprised of a set of integers (the minimum elevations) joined with an underscore ("_"). The order that the minimum elevations are joined should match the order that the ground stations were joined. That is, if the <span style="font-style: italic;">ground_stations</span> parameter is "FXB_SRI" and the <span style="font-style: italic;">min_elevations</span> parameter is "30_45", FXB will have a minimum elevation of 30&deg; and SRI will have a minimum elevation of 45&deg;. If not specified or if there are more ground stations than minimum elevations, a default value will be used.</td>
      <td colspan='1'><span style="font-style: italic;">/passes/RAX-2.json?ground_stations=FXB_SRI&min_elevations=30_45</span></td>
    </tr>
  </tbody>
</table>
<p>The resources that each of these parameters can be used on is indicated in the "Works With" column.</p>

<h3 class="titles"><a name="resource_sources"><a href="#resource_sources" class="doc_link">1.4 Sources Resources: /api/sources/[sources].[format]</a></a></h3>
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
          "epoch":"261.50866133",
          "ftd_mm_d2":".00000180",
          "std_mm_d6":"00000-0",
          "bstar_drag":"10239-3",
          "element_number":"494",
          "checksum_l1":"1",
          "inclination":"98.6965",
          "right_ascension":"268.9254",
          "eccentricity":"0009041",
          "perigee":"298.0471",
          "mean_anomaly":"61.9785",
          "mean_motion":"14.21328962",
          "revs":"47812",
          "checksum_l2":"5",
          "raw_l1":"1 27842U 03031C   12261.50866133  .00000180  00000-0  10239-3 0  4941",
          "raw_l2":"2 27842  98.6965 268.9254 0009041 298.0471  61.9785 14.21328962478125"
        },
        ...
      },
      "status":{
        "url":"http:\/\/celestrak.com\/NORAD\/elements\/cubesat.txt",
        "description":"Cubesat satellites.",
        "updated":1347976291,
        "satellites_fetched":39
      }
    },
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
          "epoch":"261.20237825",
          "ftd_mm_d2":".00000021",
          "std_mm_d6":"00000-0",
          "bstar_drag":"10000-3",
          "element_number":"705",
          "checksum_l1":"1",
          "inclination":"54.4816",
          "right_ascension":"234.9334",
          "eccentricity":"0119386",
          "perigee":"329.0989",
          "mean_anomaly":"30.1776",
          "mean_motion":"2.00574017",
          "revs":"15973",
          "checksum_l2":"6",
          "raw_l1":"1 20959U 90103A   12261.20237825  .00000021  00000-0  10000-3 0  7051",
          "raw_l2":"2 20959  54.4816 234.9334 0119386 329.0989  30.1776  2.00574017159736"
        },
        ...
      },
      "status":{
        "url":"http:\/\/www.celestrak.com\/NORAD\/elements\/gps-ops.txt",
        "description":"US GPS satellites.",
        "updated":1347976291,
        "satellites_fetched":31
      }
    }
  },
  "status":{
    "status":"okay",
    "message":"At least one of the specified sources was loaded.",
    "timestamp":1347988683,
    "sources_fetched":1,
    "params":{
      "timestamp":1347988683,
      "sources":["CUBESAT","GPS"]
    }
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
    &lt;timestamp&gt;1347989842&lt;/timestamp&gt;
    &lt;sources_fetched&gt;2&lt;/sources_fetched&gt;
    &lt;params&gt;
      &lt;timestamp&gt;1347989842&lt;/timestamp&gt;
      &lt;sources&gt;
        &lt;source&gt;CUBESAT&lt;/source&gt;
        &lt;source&gt;GPS&lt;/source&gt;
      &lt;/sources&gt;
    &lt;/params&gt;
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

<h3 class="titles"><a name="resource_satellites"><a href="#resource_satellites" class="doc_link">1.5 Satellites Resources: /api/satellites/[satellites].[format]</a></a></h3>
<p>The <span style="font-style: italic;">satellites</span> resource allows you to retrieve the TLEs for the specified satellites. Any number of satellites can be requested by joining their identifiers together with underscores. For example, to retrieve the most recent TLEs in JSON for the "M-CUBED & EXP-1 PRIME" and "BEESAT-2" satellites this request would be used:</p>
<pre>
<?php echo Router::url('/', true); ?>api/satellites/M-CUBED%20%26%20EXP-1%20PRIME_BEESAT-2.json
</pre>
<p>As stated in the <a href="#making_request" class="link">Making An API Request</a> section, all satellite names must be URL encoded (like "M-CUBED & EXP-1 PRIME" in the example request above) before being joined with the underscore.</p>
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
  "satellites": {
    "M-CUBED & EXP-1 PRIME": [
      {
        "status": {
          "loaded": 1437451790
        },
        "tle": {
          "name": "M-CUBED & EXP-1 PRIME",
          "satellite_number": "37855",
          "classification": "U",
          "launch_year": "11",
          "launch_number": "61",
          "launch_piece": "F",
          "epoch_year": "15",
          "epoch": "200.86284827",
          "ftd_mm_d2": ".00002504",
          "std_mm_d6": "00000-0",
          "bstar_drag": "15486-3",
          "element_number": "999",
          "checksum_l1": "8",
          "inclination": "101.7136",
          "right_ascension": "70.1740",
          "eccentricity": "0193852",
          "perigee": "338.0755",
          "mean_anomaly": "21.2240",
          "mean_motion": "14.97497646",
          "revs": "20204",
          "checksum_l2": "3",
          "raw_l1": "1 37855U 11061F   15200.86284827  .00002504  00000-0  15486-3 0  9998",
          "raw_l2": "2 37855 101.7136  70.1740 0193852 338.0755  21.2240 14.97497646202043"
        }
      }
    ],
    "BEESAT-2": [
      {
        "status": {
          "loaded": 1437451790
        },
        "tle": {
          "name": "BEESAT-2",
          "satellite_number": "39136",
          "classification": "U",
          "launch_year": "13",
          "launch_number": "15",
          "launch_piece": "G",
          "epoch_year": "15",
          "epoch": "200.77422451",
          "ftd_mm_d2": ".00002816",
          "std_mm_d6": "00000-0",
          "bstar_drag": "17781-3",
          "element_number": "999",
          "checksum_l1": "6",
          "inclination": "64.8743",
          "right_ascension": "315.8957",
          "eccentricity": "0030176",
          "perigee": "246.7378",
          "mean_anomaly": "113.0568",
          "mean_motion": "15.10883197",
          "revs": "12358",
          "checksum_l2": "4",
          "raw_l1": "1 39136U 13015G   15200.77422451  .00002816  00000-0  17781-3 0  9996",
          "raw_l2": "2 39136  64.8743 315.8957 0030176 246.7378 113.0568 15.10883197123584"
        }
      }
    ]
  },
  "status": {
    "status": "okay",
    "message": "At least one TLE was returned for the specified satellites.",
    "timestamp": 1437625922,
    "satellites_fetched": 2,
    "tles_fetched": 2,
    "params": {
      "timestamp": 1437625922,
      "satellites": [
        "M-CUBED & EXP-1 PRIME",
        "BEESAT-2"
      ]
    }
  }
}
</pre>
  </div>
  <div class="tab-pane" id="satellites_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0" encoding="UTF-8"?&gt;
&lt;api_satellites&gt;
  &lt;status&gt;
    &lt;status&gt;okay&lt;/status&gt;
    &lt;message&gt;At least one TLE was returned for the specified satellites.&lt;/message&gt;
    &lt;timestamp&gt;1437626178&lt;/timestamp&gt;
    &lt;satellites_fetched&gt;2&lt;/satellites_fetched&gt;
    &lt;tles_fetched&gt;2&lt;/tles_fetched&gt;
    &lt;params&gt;
      &lt;timestamp&gt;1437626178&lt;/timestamp&gt;
      &lt;satellites&gt;
        &lt;satellite&gt;M-CUBED &amp; EXP-1 PRIME&lt;/satellite&gt;
        &lt;satellite&gt;BEESAT-2&lt;/satellite&gt;
      &lt;/satellites&gt;
    &lt;/params&gt;
  &lt;/status&gt;
  &lt;satellites&gt;
    &lt;satellite name="M-CUBED &amp; EXP-1 PRIME"&gt;
      &lt;tles&gt;
        &lt;tle&gt;
          &lt;name&gt;M-CUBED &amp; EXP-1 PRIME&lt;/name&gt;
          &lt;satellite_number&gt;37855&lt;/satellite_number&gt;
          &lt;classification&gt;U&lt;/classification&gt;
          &lt;launch_year&gt;11&lt;/launch_year&gt;
          &lt;launch_number&gt;61&lt;/launch_number&gt;
          &lt;launch_piece&gt;F&lt;/launch_piece&gt;
          &lt;epoch_year&gt;15&lt;/epoch_year&gt;
          &lt;epoch&gt;200.86284827&lt;/epoch&gt;
          &lt;ftd_mm_d2&gt;.00002504&lt;/ftd_mm_d2&gt;
          &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
          &lt;bstar_drag&gt;15486-3&lt;/bstar_drag&gt;
          &lt;element_number&gt;999&lt;/element_number&gt;
          &lt;checksum_l1&gt;8&lt;/checksum_l1&gt;
          &lt;inclination&gt;101.7136&lt;/inclination&gt;
          &lt;right_ascension&gt;70.1740&lt;/right_ascension&gt;
          &lt;eccentricity&gt;0193852&lt;/eccentricity&gt;
          &lt;perigee&gt;338.0755&lt;/perigee&gt;
          &lt;mean_anomaly&gt;21.2240&lt;/mean_anomaly&gt;
          &lt;mean_motion&gt;14.97497646&lt;/mean_motion&gt;
          &lt;revs&gt;20204&lt;/revs&gt;
          &lt;checksum_l2&gt;3&lt;/checksum_l2&gt;
          &lt;raw_l1&gt;1 37855U 11061F   15200.86284827  .00002504  00000-0  15486-3 0  9998&lt;/raw_l1&gt;
          &lt;raw_l2&gt;2 37855 101.7136  70.1740 0193852 338.0755  21.2240 14.97497646202043&lt;/raw_l2&gt;
          &lt;status&gt;
            &lt;loaded&gt;1437451790&lt;/loaded&gt;
          &lt;/status&gt;
        &lt;/tle&gt;
      &lt;/tles&gt;
    &lt;/satellite&gt;
    &lt;satellite name="BEESAT-2"&gt;
      &lt;tles&gt;
        &lt;tle&gt;
          &lt;name&gt;BEESAT-2&lt;/name&gt;
          &lt;satellite_number&gt;39136&lt;/satellite_number&gt;
          &lt;classification&gt;U&lt;/classification&gt;
          &lt;launch_year&gt;13&lt;/launch_year&gt;
          &lt;launch_number&gt;15&lt;/launch_number&gt;
          &lt;launch_piece&gt;G&lt;/launch_piece&gt;
          &lt;epoch_year&gt;15&lt;/epoch_year&gt;
          &lt;epoch&gt;200.77422451&lt;/epoch&gt;
          &lt;ftd_mm_d2&gt;.00002816&lt;/ftd_mm_d2&gt;
          &lt;std_mm_d6&gt;00000-0&lt;/std_mm_d6&gt;
          &lt;bstar_drag&gt;17781-3&lt;/bstar_drag&gt;
          &lt;element_number&gt;999&lt;/element_number&gt;
          &lt;checksum_l1&gt;6&lt;/checksum_l1&gt;
          &lt;inclination&gt;64.8743&lt;/inclination&gt;
          &lt;right_ascension&gt;315.8957&lt;/right_ascension&gt;
          &lt;eccentricity&gt;0030176&lt;/eccentricity&gt;
          &lt;perigee&gt;246.7378&lt;/perigee&gt;
          &lt;mean_anomaly&gt;113.0568&lt;/mean_anomaly&gt;
          &lt;mean_motion&gt;15.10883197&lt;/mean_motion&gt;
          &lt;revs&gt;12358&lt;/revs&gt;
          &lt;checksum_l2&gt;4&lt;/checksum_l2&gt;
          &lt;raw_l1&gt;1 39136U 13015G   15200.77422451  .00002816  00000-0  17781-3 0  9996&lt;/raw_l1&gt;
          &lt;raw_l2&gt;2 39136  64.8743 315.8957 0030176 246.7378 113.0568 15.10883197123584&lt;/raw_l2&gt;
          &lt;status&gt;
            &lt;loaded&gt;1437451790&lt;/loaded&gt;
          &lt;/status&gt;
        &lt;/tle&gt;
      &lt;/tles&gt;
    &lt;/satellite&gt;
  &lt;/satellites&gt;
&lt;/api_satellites&gt;
</pre>
  </div>
  <div class="tab-pane" id="satellites_raw">
<pre class="pre-scrollable">
M-CUBED & EXP-1 PRIME
1 37855U 11061F   15200.86284827  .00002504  00000-0  15486-3 0  9998
2 37855 101.7136  70.1740 0193852 338.0755  21.2240 14.97497646202043
BEESAT-2
1 39136U 13015G   15200.77422451  .00002816  00000-0  17781-3 0  9996
2 39136  64.8743 315.8957 0030176 246.7378 113.0568 15.10883197123584
</pre>
  </div>
</div>
<p>If any of the satellites can't be located, they will simply be omitted from the response. However, if none of the satellites are valid an error will be generated.</p>

<h3 class="titles"><a name="resource_positions"><a href="#resource_positions" class="doc_link">1.6 Satellite Position Resources: /api/positions/[satellites].[format]</a></a></h3>
<p>The <span style="font-style: italic;">positions</span> resource allows you to retrieve the estimated positions of satellites during a specified range. Satellite positions are calculated using the SGP4 propagation model. The positions of any number of satellites can be requested by joining their identifiers together with underscores. For example, to retrieve the estimated positions for the "RAX-2" and "CUTE-1.7+APD II (CO-65)" satellites during the next 24 hours in JSON, this request would be used:</p>
<pre>
<?php echo Router::url('/', true); ?>api/positions/RAX-2_CUTE-1.7%2BAPD%20II%20(CO-65).json
</pre>
<p>As stated in the <a href="#making_request" class="link">Making An API Request</a> section, all satellite names must be URL encoded (like the "CUTE-1.7+APD II (CO-65)" satellite is in the example request above) before being joined with the underscore.</p>
<p>Instead of explaining every field returned in the response, portions of the response (clipped parts of the response are indicated by a "...") from the example request above are displayed below in every available format.</p>
<ul class="nav nav-tabs" id="positions_examples">
  <li class="active"><a href="#positions_json">JSON</a></li>
  <li><a href="#positions_xml">XML</a></li>
  <li><a href="#positions_raw">Raw Satellite Positions</a></li>
</ul>
<div class="tab-content">
  <div class="tab-pane active" id="positions_json">
<pre class="prettyprint pre-scrollable">
{
  "satellites":{
    "RAX-2":{
      "positions":{
        "1345830113":{
          "timestamp":1345830113,
          "latitude":55.351346,
          "longitude":52.918618,
          "altitude":658.076145
        },
        "1345830173":{
          "timestamp":1345830173,
          "latitude":58.765731,
          "longitude":50.151246,
          "altitude":647.762654
        },
        ...
      },
      "status":{
        "status":"okay",
        "message":"Satellite positions calculated successfully.",
        "generated_at":1345830175,
        "positions_calculated":60,
        "name":"RAX-2",
        "raw_tle_line_1":"1 37853U 11061D   12235.83404004  .00003153  00000-0  25520-3 0  2645",
        "raw_tle_line_2":"2 37853 101.7092 306.3753 0246038 160.8856 200.1781 14.80426852 44256",
        "timestamp_start":1345830113,
        "timestamp_end":1345833713,
        "resolution":60
      }
    },
    "CUTE-1.7+APD II (CO-65)":{
      "positions":{
        "1345830113":{
          "timestamp":1345830113,
          "latitude":60.801292,
          "longitude":44.181864,
          "altitude":636.395418
        },
        "1345830173":{
          "timestamp":1345830173,
          "latitude":64.330966,
          "longitude":41.576805,
          "altitude":636.611687
        },
        ...
      },
      "status":{
        "status":"okay",
        "message":"Satellite positions calculated successfully.",
        "generated_at":1345830175,
        "positions_calculated":60,
        "name":"CUTE-1.7+APD II (CO-65)",
        "raw_tle_line_1":"1 32785U 08021C   12236.17412516  .00000497  00000-0  67364-4 0  3957",
        "raw_tle_line_2":"2 32785  97.7916 295.6249 0014096 198.2384 161.8322 14.83121379233693",
        "timestamp_start":1345830113,
        "timestamp_end":1345833713,
        "resolution":60
      }
    }
  },
  "status":{
    "status":"okay",
    "message":"The positions of the specified satellites were calculated successfully.",
    "timestamp":1347990223,
    "satellites_calculated":2,
    "params":{
      "start":1347990223,
      "end":1348076623,
      "resolution":60,
      "satellites":["RAX-2","CUTE-1.7+APD II (CO-65)"]
    }
  }
}
</pre>
  </div>
  <div class="tab-pane" id="positions_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0"?&gt;
&lt;api_positions&gt;
  &lt;status&gt;
    &lt;status&gt;okay&lt;/status&gt;
    &lt;message&gt;The positions of the specified satellites were calculated successfully.&lt;/message&gt;
    &lt;timestamp&gt;1347990287&lt;/timestamp&gt;
    &lt;satellites_calculated&gt;2&lt;/satellites_calculated&gt;
    &lt;params&gt;
      &lt;start&gt;1347990287&lt;/start&gt;
      &lt;end&gt;1348076687&lt;/end&gt;
      &lt;resolution&gt;60&lt;/resolution&gt;
      &lt;satellites&gt;
        &lt;satellite&gt;RAX-2&lt;/satellite&gt;
        &lt;satellite&gt;CUTE-1.7+APD II (CO-65)&lt;/satellite&gt;
      &lt;/satellites&gt;
    &lt;/params&gt;
  &lt;/status&gt;
  &lt;satellites&gt;
    &lt;satellite name='RAX-2'&gt;
      &lt;status&gt;
        &lt;status&gt;okay&lt;/status&gt;
        &lt;message&gt;Satellite positions calculated successfully.&lt;/message&gt;
        &lt;generated_at&gt;1345830822&lt;/generated_at&gt;
        &lt;positions_calculated&gt;60&lt;/positions_calculated&gt;
        &lt;name&gt;RAX-2&lt;/name&gt;
        &lt;raw_tle_line_1&gt;1 37853U 11061D   12235.83404004  .00003153  00000-0  25520-3 0  2645&lt;/raw_tle_line_1&gt;
        &lt;raw_tle_line_2&gt;2 37853 101.7092 306.3753 0246038 160.8856 200.1781 14.80426852 44256&lt;/raw_tle_line_2&gt;
        &lt;timestamp_start&gt;1345830113&lt;/timestamp_start&gt;
        &lt;timestamp_end&gt;1345833713&lt;/timestamp_end&gt;
        &lt;resolution&gt;60&lt;/resolution&gt;
      &lt;/status&gt;
      &lt;positions&gt;
        &lt;position timestamp='1345830113'&gt;
          &lt;latitude&gt;55.351346&lt;/latitude&gt;
          &lt;longitude&gt;52.918618&lt;/longitude&gt;
          &lt;altitude&gt;658.076145&lt;/altitude&gt;
        &lt;/position&gt;
        &lt;position timestamp='1345830173'&gt;
          &lt;latitude&gt;58.765731&lt;/latitude&gt;
          &lt;longitude&gt;50.151246&lt;/longitude&gt;
          &lt;altitude&gt;647.762654&lt;/altitude&gt;
        &lt;/position&gt;
        ...
      &lt;/positions&gt;
    &lt;/satellite&gt;
    &lt;satellite name='CUTE-1.7+APD II (CO-65)'&gt;
      &lt;status&gt;
        &lt;status&gt;okay&lt;/status&gt;
        &lt;message&gt;Satellite positions calculated successfully.&lt;/message&gt;
        &lt;generated_at&gt;1345830822&lt;/generated_at&gt;
        &lt;positions_calculated&gt;60&lt;/positions_calculated&gt;
        &lt;name&gt;CUTE-1.7+APD II (CO-65)&lt;/name&gt;
        &lt;raw_tle_line_1&gt;1 32785U 08021C   12236.17412516  .00000497  00000-0  67364-4 0  3957&lt;/raw_tle_line_1&gt;
        &lt;raw_tle_line_2&gt;2 32785  97.7916 295.6249 0014096 198.2384 161.8322 14.83121379233693&lt;/raw_tle_line_2&gt;
        &lt;timestamp_start&gt;1345830113&lt;/timestamp_start&gt;
        &lt;timestamp_end&gt;1345833713&lt;/timestamp_end&gt;
        &lt;resolution&gt;60&lt;/resolution&gt;
      &lt;/status&gt;
      &lt;positions&gt;
        &lt;position timestamp='1345830113'&gt;
          &lt;latitude&gt;60.801292&lt;/latitude&gt;
          &lt;longitude&gt;44.181864&lt;/longitude&gt;
          &lt;altitude&gt;636.395418&lt;/altitude&gt;
        &lt;/position&gt;
        &lt;position timestamp='1345830173'&gt;
          &lt;latitude&gt;64.330966&lt;/latitude&gt;
          &lt;longitude&gt;41.576805&lt;/longitude&gt;
          &lt;altitude&gt;636.611687&lt;/altitude&gt;
        &lt;/position&gt;
        ...
      &lt;/positions&gt;
    &lt;/satellite&gt;
  &lt;/satellites&gt;
&lt;/api_positions&gt;
</pre>
  </div>
  <div class="tab-pane" id="positions_raw">
  <div style="font-style: italic;margin-bottom: 10px;">[satellite name]:[timestamp]:[latitude]:[longitude]:[altitude]</div>
<pre class="pre-scrollable">
RAX-2:1345830113:55.351346:52.918618:658.076145
RAX-2:1345830173:58.765731:50.151246:647.762654
..
CUTE-1.7+APD II (CO-65):1345830113:60.801292:44.181864:636.395418
CUTE-1.7+APD II (CO-65):1345830173:64.330966:41.576805:636.611687
..
</pre>
  </div>
</div>
<p>If any of the satellites can't be located, they will simply be omitted from the response. However, if one or more of the valid satellites generates an error (e.g. calculation error), the whole request will return an error response.</p>

<h3 class="titles"><a name="resource_passes"><a href="#resource_passes" class="doc_link">1.7 Satellite Pass Prediction Resource: /api/passes/[satellite].[format]</a></a></h3>
<p>The <span style="font-style: italic;">passes</span> resource allows you to retrieve a satellite's predicted pass times over specified ground stations. You can also specify minimum elevations for each ground station, a prediction start date, and various result filters. For example, to retrieve the next 5 acceptable passes (i.e. passes that meet the minimum elevation requirements) for the "RAX-2" satellite over FXB in Ann Arbor  with a minimum elevation of 20 degrees in JSON, this request would be used:</p>
<pre>
<?php echo Router::url('/', true); ?>api/passes/CUTE-1.7%2BAPD%20II%20%28CO-65%29.json?pass_count=5&ground_stations=FXB&min_elevations=20&show_all_passes=false
</pre>
<p>As stated in the <a href="#making_request" class="link">Making An API Request</a> section, all ground station and satellite names names must be URL encoded (like the "CUTE-1.7+APD II (CO-65)" satellite is in the example request above) before being joined with the underscore.</p>
<p>This API also features a user-friendly <a class="link" target="_blank" href="<?php echo Router::url('/', true); ?>tools/passes">pass-time calculation tool</a>.</p>
<p>Instead of explaining every field returned in the response, portions of the response (clipped parts of the response are indicated by a "...") from the example request above are displayed below in every available format.</p>
<ul class="nav nav-tabs" id="passes_examples">
  <li class="active"><a href="#passes_json">JSON</a></li>
  <li><a href="#passes_xml">XML</a></li>
  <li><a href="#passes_raw">Raw Satellite Passes</a></li>
</ul>
<div class="tab-content">
  <div class="tab-pane active" id="passes_json">
<pre class="prettyprint pre-scrollable">
{
  "passes":[
    {
      "pass":{
        "orbit_number":23754,
        "aos":1347933717,
        "aos_az":147,
        "mel":1347934097,
        "mel_az":70,
        "los":1347934470,
        "los_az":356,
        "acceptable_el_start":1347933950,
        "acceptable_el_end":1347934252,
        "duration":"00:12:33",
        "peak_elevation":39.6,
        "acceptable":true,
        "ground_station":"FXB"
      }
    },
    ...
  ],
  "status":{
    "status":"okay",
    "message":"Passes were computed successfully.",
    "timestamp":1348008751,
    "total_passes_loaded":5,
    "acceptable_passes_loaded":5,
    "params":{
      "pass_count":5,
      "timestamp":1348008751,
      "show_all_passes":false,
      "satellite":"CUTE-1.7+APD II (CO-65)",
      "raw_tle_line_1":"1 32785U 08021C   12261.27121132  .00001098  00000-0  14065-3 0  4179",
      "raw_tle_line_2":"2 32785  97.7893 320.1075 0016031 119.4239 240.8600 14.83187504237416",
      "ground_stations":{
        "FXB":{
          "min_elevation":20,
          "name":"FXB",
          "latitude":42.293385,
          "longitude":-83.712076,
          "description":"FXB Ground Station in Ann Arbor, MI"
        }
      }
    }
  }
}
</pre>
  </div>
  <div class="tab-pane" id="passes_xml">
<pre class="prettyprint pre-scrollable">
&lt;?xml version="1.0"?&gt;
&lt;api_passes&gt;
  &lt;status&gt;
    &lt;status&gt;okay&lt;/status&gt;
    &lt;message&gt;Passes were computed successfully.&lt;/message&gt;
    &lt;timestamp&gt;1348009662&lt;/timestamp&gt;
    &lt;total_passes_loaded&gt;5&lt;/total_passes_loaded&gt;
    &lt;acceptable_passes_loaded&gt;5&lt;/acceptable_passes_loaded&gt;
    &lt;params&gt;
      &lt;pass_count&gt;5&lt;/pass_count&gt;
      &lt;timestamp&gt;1348009662&lt;/timestamp&gt;
      &lt;show_all_passes&gt;false&lt;/show_all_passes&gt;
      &lt;satellite&gt;CUTE-1.7+APD II (CO-65)&lt;/satellite&gt;
      &lt;raw_tle_line_1&gt;1 32785U 08021C   12261.27121132  .00001098  00000-0  14065-3 0  4179&lt;/raw_tle_line_1&gt;
      &lt;raw_tle_line_2&gt;2 32785  97.7893 320.1075 0016031 119.4239 240.8600 14.83187504237416&lt;/raw_tle_line_2&gt;
      &lt;ground_stations&gt;
        &lt;ground_station name='FXB'&gt;
          &lt;name&gt;FXB&lt;/name&gt;
          &lt;min_elevation&gt;20&lt;/min_elevation&gt;
          &lt;latitude&gt;42.293385&lt;/latitude&gt;
          &lt;longitude&gt;-83.712076&lt;/longitude&gt;
          &lt;description&gt;FXB Ground Station in Ann Arbor, MI&lt;/description&gt;
        &lt;/ground_station&gt;
      &lt;/ground_stations&gt;
    &lt;/params&gt;
  &lt;/status&gt;
  &lt;passes&gt;
    &lt;pass aos='1347933717'&gt;
      &lt;orbit_number&gt;23754&lt;/orbit_number&gt;
      &lt;aos&gt;1347933717&lt;/aos&gt;
      &lt;aos_az&gt;147&lt;/aos_az&gt;
      &lt;mel&gt;1347934097&lt;/mel&gt;
      &lt;mel_az&gt;70&lt;/mel_az&gt;
      &lt;los&gt;1347934470&lt;/los&gt;
      &lt;los_az&gt;356&lt;/los_az&gt;
      &lt;acceptable_el_start&gt;1347933950&lt;/acceptable_el_start&gt;
      &lt;acceptable_el_end&gt;1347934252&lt;/acceptable_el_end&gt;
      &lt;duration&gt;00:12:33&lt;/duration&gt;
      &lt;peak_elevation&gt;39.6&lt;/peak_elevation&gt;
      &lt;acceptable&gt;true&lt;/acceptable&gt;
      &lt;ground_station&gt;FXB&lt;/ground_station&gt;
    &lt;/pass&gt;
    ...
  &lt;/passes&gt;
&lt;/api_passes&gt;
</pre>
  </div>
  <div class="tab-pane" id="passes_raw">
  <div style="font-style: italic;margin-bottom: 10px;">[satellite name]#[orbit]#[aos]#[aos_az]#[mel]#[mel_az]#[los]#[los_az]#[acceptable_el_start]#[acceptable_el_end]#[duration]#[peak_elevation]#[acceptable]#[station]</div>
<pre class="pre-scrollable">
CUTE-1.7+APD II (CO-65)#23769#1348021126#157#1348021515#71#1348021903#351#1348021344#1348021344#00:12:57#61.2#1#FXB
CUTE-1.7+APD II (CO-65)#23778#1348069274#10#1348069655#281#1348070043#200#1348069484#1348069484#00:12:49#69.4#1#FXB
CUTE-1.7+APD II (CO-65)#23784#1348108544#167#1348108933#265#1348109321#347#1348108754#1348108754#00:12:57#86.1#1#FXB
CUTE-1.7+APD II (CO-65)#23793#1348156700#5#1348157073#286#1348157454#210#1348156918#1348156918#00:12:33#44.4#1#FXB
CUTE-1.7+APD II (CO-65)#23799#1348195970#177#1348196351#257#1348196739#343#1348196188#1348196188#00:12:49#55.1#1#FXB
</pre>
  </div>
</div>
<p><span style="font-style: italic;">acceptable_el_start</span> and <span style="font-style: italic;">acceptable_el_end</span> indicate the start and end times of the period when the pass is above the specified elevation (assuming it is an acceptable pass). </p>

<p>If any of the specified ground stations can't be found, an error will be generated.</p>

<h3 class="titles"><a name="errors"><a href="#errors" class="doc_link">1.8 API Errors</a></a></h3>
<p>The top level status element for the sources, satellites, and positions resources will always be returned, even in the event of an error (although this will not occur with the occasional server error). For example, the result of requesting a single non-existent satellite would be:</p>
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
  "timestamp":1347990409,
  "satellites_fetched":0,
  "params":{
    "timestamp":1347990409,
    "satellites":["DOESNTEXIST"]
  }
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
    &lt;timestamp&gt;1347990456&lt;/timestamp&gt;
    &lt;satellites_fetched&gt;0&lt;/satellites_fetched&gt;
    &lt;params&gt;
      &lt;timestamp&gt;1347990456&lt;/timestamp&gt;
      &lt;satellites&gt;
        &lt;satellite&gt;DOESNTEXIST&lt;/satellite&gt;
      &lt;/satellites&gt;
    &lt;/params&gt;
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
<h2 class="titles"><a name="available_satellites"><a href="#available_satellites" class="doc_link">2.0 Available Sources, Satellites, and Ground Stations</a></a></h2>
<p>Below, lists of the available sources and their satellites along with their URL encoding are displayed for your convenience.</p>
<?php if (!empty($sources)): ?>
  <table class="table table-hover">
    <thead>
      <tr>
        <th width="15%">Source Name</th>
        <th width="15%">URL Encoded Name</th>
        <th width="20%">Last Updated</th>
        <th width="15%">Description</th>
        <th width="35%">URL</th>
      </tr>
    </thead>
    <tbody>
      <?php foreach($sources as $source): ?>
        <tr id="source_<?php echo $source['Source']['id']; ?>">
          <td colspan="1"><?php echo $source['Source']['name']; ?> <a class="expand_link" rel="<?php echo $source['Source']['id']; ?>">[+ Satellites]</a></td>
          <td colspan="1"><?php echo rawurlencode($source['Source']['name']); ?></td>
          <td colspan="1">
          <?php if (isset($source['Update'][0]['created_on'])): ?>
            <?php echo date('m/d/Y G:i:s T', $source['Update'][0]['created_on']); ?>
          <?php else: ?>
            <span style="font-style: italic;">Not Updated Yet</span>
          <?php endif; ?>
          </td>
          <td colspan="1"><?php echo $source['Source']['description']; ?></td>
          <td colspan="1"><a href="<?php echo $source['Source']['url']; ?>" target="_blank" class="link"><?php echo $source['Source']['url']; ?></a></td>
        </tr>
        <tr id="satellites_<?php echo $source['Source']['id']; ?>" class="satellite_row">
          <td colspan="1"></td>
          <td colspan="4">
          <?php if (!empty($source['Update'][0]['Tle'])): ?>
            <table style="width: 50%;" class="table table-condensed satellite_table">
              <thead>
                <th>Satellite Name</th>
                <th>Encoded Satellite Name</th>
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
<p>The list of all available ground stations is displayed below. These are typically used with the "passes" API.</p>
<?php if (!empty($stations)): ?>
  <table class="table table-hover">
    <thead>
      <tr>
        <th width="15%">Station Name</th>
        <th width="15%">URL Encoded Name</th>
        <th width="10%">Latitude</th>
        <th width="10%">Longitude</th>
        <th>Description</th>
      </tr>
    </thead>
    <tbody>
      <?php foreach($stations as $station): ?>
        <tr>
          <td colspan="1"><?php echo $station['Station']['name']; ?></td>
          <td colspan="1"><?php echo rawurlencode($station['Station']['name']); ?></td>
          <td colspan="1"><?php echo $station['Station']['latitude']; ?> <?php if ($station['Station']['latitude']>0): ?>N<?php else: ?>S<?php endif; ?></td>
          <td colspan="1"><?php echo $station['Station']['longitude']; ?> <?php if ($station['Station']['longitude']>0): ?>W<?php else: ?>E<?php endif; ?></td>
          <td colspan="1"><?php echo $station['Station']['description']; ?> <a href="https://maps.google.com/maps?q=<?php echo $station['Station']['latitude']; ?>,<?php echo $station['Station']['longitude']; ?>" target="_blank" class="link">(View Map)</a></td>
        </tr>
      <?php endforeach; ?>
    </tbody>
  </table>
<?php else: ?>
  <span style="font-style: italic;">No ground stations are currently configured. Please try again once some have been added.</span>
<?php endif; ?>
