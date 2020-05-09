function uploadLAS(file) {
    console.log(file);

    var reader = new FileReader();
    reader.onerror = function() {
        alert("error reading las/laz");
    }
    reader.onload = function(evt) {
        var lasFile = new LASFile(reader.result, file[0].name);
        console.log(`Opened LAS/LAZ file '${file[0].name}' containing ${lasFile.numPoints} points`);
        console.log(`Bounds = ${lasFile.bounds}`);

        lasFile.loadPoints();
        console.log(`Loaded ${lasFile.numLoadedPoints} (noise discarded)`);
        console.log(lasFile.positions);
        if (lasFile.hasColors) {
            console.log("File has colors");
            console.log(lasFile.colors);
        }
    }
    reader.readAsArrayBuffer(file[0]);
}

