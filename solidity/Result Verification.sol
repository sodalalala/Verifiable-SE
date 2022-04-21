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
    uint [][500] arrW =[ ];
    struct Tag{
        uint [500] A;
        uint [500] wit;
    }
    
    function Setup(Functions F) public returns(Tag memory){
    Tag memory tags;
    for (uint i=0; i<500; i++){
        tags.A[i] = F.AccumuGen(N, u, arrW[i]);
        tags.wit[i] = F.WitGen(N, u, x, arrW[i]);
    }
    return tags;
    }
    
    function Verify(Functions F,uint [] memory resultids) public returns(bool){
    Tag memory tags;
    uint id;
    tags = Setup(F);
    for (uint i=0; i<resultids.length; i++){
        id = resultids[i];
        if (F.AccVerify(N, x, tags.wit[id], tags.A[id])){
            arr.push(id);
        }
    }
    if(arr.length == resultids.length){
    return true;
    }
    }
}