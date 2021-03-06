var nodeIn = [];
var nodeOut = [];
var nodeChildren = [];
var max_depth = [];
var span_count_in = [];
var span_count_out = [];
var current_depth = -1;

function alignIn( sentence, word, depth ) {
  var id, i;
  if (current_depth < depth) {
    current_depth = depth;
  }
  else { return; }

  id = 0;
  for(i=1;i<nodeIn[sentence].size();i++ ) {
    if (nodeIn[sentence][i].start <= word && word <= nodeIn[sentence][i].end && nodeIn[sentence][i].depth <= depth) {
      id = i;
    }
  }

  highlightNode( sentence, id );
}

function alignOut( sentence, word, depth ) {
  var id, i;
  if (current_depth < depth) {
    current_depth = depth;
  }
  else { return; }
  id = 0;
  for(i=1;i<nodeOut[sentence].size();i++ ) {
    if (nodeOut[sentence][i].start <= word && word <= nodeOut[sentence][i].end && nodeOut[sentence][i].depth <= depth) {
      id = i;
    }
  }

  highlightNode( sentence, id );
}

function unAlign( sentence ) {
  if (current_depth == -1) { return; }
  current_depth = -1;
  lowlightAllNodes( sentence );
}

function highlightNode( sentence, id ) {
  var i;
  lowlightAllNodes( sentence );
  highlightSingleNode( sentence, id, 'yellow' );
  for(i=0; i<nodeChildren[sentence][id].size(); i++) {
    var childId = nodeChildren[sentence][id][i];
    var j;
    highlightSingleNode( sentence, childId, '#ffffa0');
    for(j=0; j<nodeChildren[sentence][childId].size(); j++) {
      highlightSingleNode( sentence, nodeChildren[sentence][childId][j], '#ffffe0');
    }
  }
}

function highlightSingleNode( sentence, id, color ) {
  var i, j;
  for(i=nodeIn[sentence][id].start;i<=nodeIn[sentence][id].end;i++) {
    for(j=nodeIn[sentence][id].depth;j<=max_depth[sentence];j++) {
      var item = "in-" + sentence + "-" + i + "-" + j;
      if ($(item) != null) {
        $(item).setStyle({ backgroundColor: color, borderColor: 'red' });
      }
    }
  }
  //$("debug").innerHTML = "highlight: "+id+", of "+nodeOut[sentence].size()+"<br>";
  for(i=nodeOut[sentence][id].start;i<=nodeOut[sentence][id].end;i++) {
    for(j=nodeOut[sentence][id].depth;j<=max_depth[sentence];j++) {
      var item = "out-" + sentence + "-" + i + "-" + j;
      //$("debug").innerHTML += item;
      if ($(item) != null) {
        $(item).setStyle({ backgroundColor: color, borderColor: 'red' });
      }
    }
  }
}

function lowlightAllNodes( sentence ) {
  var i, j;
  for(i=0;i<span_count_in[sentence];i++) {
    for(j=0;j<=max_depth[sentence];j++) {
      var item = "in-" + sentence + "-" + i + "-" + j;
      if ($(item) != null) {
        $(item).setStyle({ backgroundColor: 'white', borderColor: 'black' });
      }
    }
  }
  for(i=0;i<span_count_out[sentence];i++) {
    for(j=0;j<=max_depth[sentence];j++) {
      var item = "out-" + sentence + "-" + i + "-" + j;
      if ($(item) != null) {
        $(item).setStyle({ backgroundColor: 'white', borderColor: 'black' });
      }
    }
  }
}
