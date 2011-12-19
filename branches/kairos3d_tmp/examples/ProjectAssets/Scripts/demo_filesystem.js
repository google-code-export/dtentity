include_once("Scripts/stdlib.js");


////////////////////////////////////////////////////////////////////////////////
function startFileSystem() {
  var text = "Contents of file:";

  var infile = new File("delta3d_log.html");
  infile.open("r");
  var contents = infile.read(200).toString("ISO-8859-1", 0, 200);
  infile.close();

  var outfile = new File("temp.txt");
  outfile.open("w");
  outfile.write("Hello, File World!");
  outfile.close();
  showHelp(text + contents);

}

////////////////////////////////////////////////////////////////////////////////
function stopFileSystem() {
  hideHelp();  
}

addDemo("FileSystem", startFileSystem, stopFileSystem);
