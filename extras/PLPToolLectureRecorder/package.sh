javac -cp ../../reference/sw/PLPTool/dist/PLPTool.jar LectureRecorder.java
zip LectureRecorder.jar *.class
#scp LectureRecorder.jar rome.ceat.okstate.edu:/home/fritz/plp/goodies
rm *.class
