var script = document.createElement('script');
script.src = 'https://code.jquery.com/jquery-3.4.1.min.js';
script.type = 'text/javascript';
document.getElementsByTagName('head')[0].appendChild(script);


function fillObstacle(document, oIds) {
  for (var i = 0; i < oIds.length; i++) {
    var id = oIds[i];
    console.log("-------print id---------")
    console.log(id)
    var parts = id.split('9');

    var cell = document.getElementById(parts[0]);
    console.log("------color-----")
    console.log(parts[0])
    console.log("---------gid---------")
    console.log(parts[1])
    var color = {
      "1": "or.png",
      "2": "og.png",
      "3": "oo.png",
      "4": "ob.png",
      "5": "obl.png",
    };
    cell.firstElementChild.src = color[parts[1]];
    cell.style.backgroundColor = "#ffc107";
  }
}

function fillCar(document, carId, direction) {
  var dirs = {
    "0": "car_u.png",
    "1": "car_r.png",
    "2": "car_d.png",
    "3": "car_l.png",
  };
  var cell = document.getElementById(carId);
  cell.firstElementChild.src = dirs[direction];
  cell.style.backgroundColor = "#ffc107";
}

function fillOneCell(document, id) {
  // <div class="col">
  //   <div class="p-3 border" id="1-3" style="background-color: #198754;"></div>
  // </div>
  var cell = document.createElement('div');
  cell.className = 'col';

  var cellData = document.createElement('div')
  cellData.id = id;
  cellData.className = "p-3 border";

  var blankImage = document.createElement('img');
  blankImage.width = 50;
  blankImage.src = 'blank.png'
  blankImage.style = "margin-left: auto; margin-right: auto; display: block;"

  cellData.style = 'background-color: #198754;'
  cellData.appendChild(blankImage);

  cell.appendChild(cellData);

  var table = document.getElementById("map");
  table.appendChild(cell);

}

function fillCells(document, maxRow, maxCol) {
  for (var row = 1; row <= maxRow; row++) {
    for (var col = 1; col <= maxCol; col++) {
      var cellId = `${row}-${col}`
      fillOneCell(document, cellId)
    }
  }
}

function refreshTable(document, maxRow, maxCol) {
  console.log('refreshing ...')
  // var fluid = document.getElementsByClassName("container-fluid");
  var fluid = document.getElementById("root");

  var table = document.getElementById("map");
  if (table != null) {
    fluid.removeChild(table);
  }
  var table = document.createElement('div');
  // table.className = 'row row-cols-4 row-cols-lg-5 gy-0 g-0';
  table.className = `row row-cols-${maxRow} row-cols-lg-${maxCol} gy-0 g-0`;
  table.id = "map";

  fluid.appendChild(table);
  fillCells(document, maxRow, maxCol)
}
