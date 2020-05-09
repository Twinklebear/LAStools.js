var openLAS = Module.cwrap("open_las", "number", ["string"]);
var closeLAS = Module.cwrap("close_las", null, ["number"]);
var loadPoints = Module.cwrap("load_points", "number", ["number", "number"]);
var getNumPoints = Module.cwrap("get_num_points", "number", ["number"]);
var getNumLoadedPoints = Module.cwrap("get_num_loaded_points", "number", ["number"]);
var getBounds = Module.cwrap("get_bounds", "number", ["number"]);
var getPositions = Module.cwrap("get_positions", "number", ["number"]);
var getColors = Module.cwrap("get_colors", "number", ["number"]);
var hasColors = Module.cwrap("has_colors", "number", ["number"]);

class LASFile {
    // Construct the LASFile given some JS side ArrayBuffer data for the file and a file name
    constructor(fileData, fileName) {
        FS.writeFile(fileName, new Uint8Array(fileData));
        this.lasFile = openLAS(fileName);
        this.fileName = fileName;
    }

    loadPoints() {
        loadPoints(this.lasFile, true);
        this.positions = new Float32Array(HEAPF32.buffer,
            getPositions(this.lasFile),
            getNumLoadedPoints(this.lasFile) * 3);
        if (hasColors(this.lasFile)) {
            this.colors = new Uint8Array(HEAPU8.buffer,
                getColors(this.lasFile),
                getNumLoadedPoints(this.lasFile) * 4);
        }
    }

    close() {
        this.positions = null;
        this.colors = null;
        closeLAS(this.lasFile);
        FS.unlink(this.fileName);
        this.lasFile = null;
    }

    get bounds() {
        return new Float32Array(HEAPF32.buffer, getBounds(this.lasFile), 6);
    }

    get numLoadedPoints() {
        return getNumLoadedPoints(this.lasFile);
    }

    get numPoints() {
        return getNumPoints(this.lasFile);
    }

    get hasColors() {
        return hasColors(this.lasFile);
    }
}

