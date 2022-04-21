pragma solidity >=0.4.24 <=0.7.5;

contract Functions{
     // 快速幂取余算法
    
    function AccumuGen(uint N, uint u, uint[] memory arrW) pure public returns (uint) {
    uint A = u;
    for (uint i=0; i<arrW.length; i++){
        A = (A**arrW[i])%N;
    }
    return A;
    }
    
    function WitGen(uint N, uint u, uint[] memory arrW) pure public returns (uint) {
    uint wit=u;
    for (uint i=0; i<arrW.length-1; i++){
            wit = (wit**arrW[i])%N;
            }
    return wit;
    }
    
    function AccVerify(uint N, uint x, uint wit, uint A) pure public returns (bool){
        uint A_;
        A_ = (wit**x)%N;
        if (A==A_){
            return true;
        }
        else
            return false;
    }
    
}
