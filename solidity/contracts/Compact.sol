// SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.9.0;
pragma experimental ABIEncoderV2;

import "./Pairing.sol";

/// @title Public Compact Proof of Retrievability Implemantion
/// @notice This smart contract is developed for testing purpose only.
///         There are no rule implemented in this version to secure the
///         protocol between the client and the server.
///         Read Compact Proofs of Retrievability for more information.
/// @author Lucas GICQUEL
contract Compact {
    
    // to control access to specific functions, I store the server address
    address server;

    // client metadata used to verify each proof
    Pairing.G2Point public_key;
    Pairing.G1Point u;
    uint name;
    uint file_size;

    /**
     * A challenge, sent by anyone but the server, is defined by two integers:
     *      - i: i-th bit of file to challenge (0 <= i < file_size)
     *      - nu: independant factor chosen by the client, making each
     *           challenge unique and unpredictable
     */
    struct Challenge {
        uint i;
        uint nu;
    }

    /**
     * Proof calculated by the server, it mathematically proves that she truly
     * store the file.
     * Valid state is true if the proof is valid, false otherwise.
     */
    struct Proof {
        uint mu;
        Pairing.G1Point sigma;
        bool valid;
    }

    uint[] challenges;
    mapping(uint => Challenge) public index_challenges;

    uint[] proofs;
    mapping(uint => Proof) public index_proofs;

    /// @notice Called by a client when she wants to store her file in a
    ///         distant server.
    ///         She sends all metadata in this part allowing the smart contract
    ///         to verify the proofs calculated by the server.
    /// @param _pk_x abscissa of the public key
    /// @param _pk_y ordinate of the public key
    /// @param _u_x abscissa of a point u used both to sign and verify
    /// @param _u_y ordinate of a point u used both to sign and verify
    /// @param _name integer used as file identifier
    /// @param _file_size size of the original in bytes
    constructor(
        address _server,
        uint[2] memory _pk_x,
        uint[2] memory _pk_y,
        uint _u_x,
        uint _u_y,
        uint _name,
        uint _file_size
    )
        public
    {
        if (_file_size == 0) {
            revert("File size must be superior to 0");
        }

        if (_server == address(0) || _server == msg.sender) {
            revert("Server address invalid.");
        }

        server = _server;
        public_key = Pairing.G2Point(_pk_x, _pk_y);
        u = Pairing.G1Point(_u_x, _u_y);
        name = _name;
        file_size = _file_size;
    }

    /// @notice Anybody but the server calls this function in order to submit a
    ///         new challenge to server. Therefore, the server must answer this
    ///         challenge by a corresponding proof.
    ///         Adds the given challenge into the Challenge list.
    /// @param _i indicates the i-th bit of the original file to challenge
    /// @param _nu factor chosen by the client making challenge unpredictable
    function challenge(uint _i, uint _nu) public {
        if (msg.sender == server) {
            revert("The server is not able to call this function");
        }

        uint id = challenges.push(challenges.length);

        // i must NOT be superior to the file size
        index_challenges[id] = Challenge(_i % file_size, _nu);
    }

    /// @notice Mathematical answer to the last challenge sent.
    ///         This function can only be called by the server.
    /// @param _mu First part of the proof
    /// @param _sigma_x Abscissa of the second part of the proof
    /// @param _sigma_y Ordinate of the second part of the proof
    function proof(uint _mu, uint _sigma_x, uint _sigma_y) public {
        if (msg.sender != server) {
            revert("Only the server can call this function");
        }

        uint id = proofs.push(proofs.length);

        // i must NOT be superior to the file size
        index_proofs[id] = Proof(_mu, Pairing.G1Point(_sigma_x, _sigma_y), false);
        verify();
    }

    /// @notice Verifies if the proof sent by the server is valid according to
    ///         the last challenge sent by the client.
    ///         The valid statement will be updated depending on the
    ///         correctness of the proof.
    ///         Internal function, automatically called after the proof is sent
    function verify() internal {
        Proof storage p = index_proofs[proofs.length];
        Challenge memory c = index_challenges[challenges.length];
        Pairing.G1Point memory h = Pairing.curveMul(Pairing.P1(), (name * c.i));
        h = Pairing.curveMul(h, c.nu);
        Pairing.G1Point memory res = Pairing.curveMul(u, p.mu);
        res = Pairing.addition(res, h);

        p.valid = 
            Pairing.pairing2(
                Pairing.negate(p.sigma),
                Pairing.P2(),
                res,
                public_key
            );
    }

    function getChallengesLength() public view returns(uint) {
        return challenges.length;
    }

    function getProofsLength() public view returns(uint) {
        return proofs.length;
    }
}