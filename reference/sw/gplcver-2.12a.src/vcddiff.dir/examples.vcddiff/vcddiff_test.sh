#same files no differences 
VCDDIFF=../../bin/vcddiff
if test ! -f $VCDDIFF
then
  echo "There is no vcddiff in ../../bin"
  echo "Must be made in ../src" 
  exit;
fi
echo "============= VCDDIFF SAME FILES ===================="
$VCDDIFF example.dump example.dump 
echo "============== COMPLETED Same file test completed ==========="

#same files no differences, just different identifiers 
echo "============= VCDDIFF MAPPING FILE ===================="
$VCDDIFF example.dump example_map_nochg.dump 
echo "============== COMPLETED mapping file =============="

#print difference between files 
echo "============= VCDDIFF DIFFERENT FILES ===================="
$VCDDIFF example.dump example2.dump
echo "============== COMPLETED different files ==========="

#print difference between files 
echo "============= VCDDIFF DIFFERENT (STATE) FILES ===================="
$VCDDIFF --state example.dump example2.dump
echo "============== COMPLETED (STATE) different files ==========="
echo ""
echo "sample runs/output have been displayed"
