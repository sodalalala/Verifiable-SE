pragma solidity >=0.4.24 <=0.8.0;
pragma experimental ABIEncoderV2;
import "./Functions.sol";


//验证过程 

contract ResultVerify{
    int a;
    uint[] arr;
    uint x = 5;
    uint N = 5767;
    uint u = 2;
    uint [][500] arrWW =[];
    struct Tag{
        uint [500] A;
        uint [500] wit;
    }
    
    function AccumuGen(uint[] memory arrW) view public returns (uint) {
    uint A = u;
    for (uint i=0; i<arrW.length; i++){
        A = (A**arrW[i])%N;
    }
    return A;
    }
    
    function WitGen(uint[] memory arrW) view public returns (uint) {
    uint wit=u;
    for (uint i=0; i<arrW.length-1; i++){
            wit = (wit**arrW[i])%N;
            }
    return wit;
    }
    
    function AccVerify(uint wit, uint A) view public returns (bool){
        uint A_;
        A_ = (wit**x)%N;
        if (A==A_){
            return true;
        }
        else
            return false;
    }
    
    function Setup() public returns(Tag memory){
    Tag memory tags;
    for (uint i=0; i<300; i++){
        tags.A[i] = AccumuGen(arrWW[i]);
        tags.wit[i] = WitGen(arrWW[i]);
    }
    return tags;
    }
    
    function Verify(uint [500] memory resultids) public returns(bool){
    Tag memory tags;
    tags = Setup();
    for (uint i=0; i<resultids.length; i++){
        if (AccVerify(tags.wit[i], tags.A[i])){
            arr.push(i);
        }
    }
    if(arr.length == resultids.length){
    return true;
    }
    }
}