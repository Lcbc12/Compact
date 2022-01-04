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
    Pairing.G1Point[] u;
    uint name;
    uint file_size;

    /**
     * A challenge, sent by anyone but the server, is defined by two list of
     * integers:
     *      - i: i-th bit of file to challenge (0 <= i < file_size)
     *      - nu: independant factor chosen by the client, making each
     *           challenge unique and unpredictable
     */
    struct Challenge {
        uint[] i;
        uint[] nu;
    }

    /**
     * Proof calculated by the server, it mathematically proves that she truly
     * store the file.
     * Valid state is true if the proof is valid, false otherwise.
     */
    struct Proof {
        uint[] mu;
        Pairing.G1Point sigma;
        bool valid;
    }

    /**
     * To store and facilitate information retrieval 
     */
    uint[] challenges;
    mapping(uint => Challenge) index_challenges;

    uint[] proofs;
    mapping(uint => Proof) index_proofs;

    /// @notice Called by a client when she wants to store her file in a
    ///         distant server.
    ///         She sends all metadata in this part allowing the smart contract
    ///         to verify the proofs calculated by the server.
    /// @param _pk_x abscissa of the public key
    /// @param _pk_y ordinate of the public key
    /// @param _u_x list of abscissas of points u used both to sign and verify
    /// @param _u_y list of ordinates of points u used both to sign and verify
    /// @param _name integer used as file identifier
    /// @param _file_size size of the original in bytes
    constructor(
        address _server,
        uint[2] memory _pk_x,
        uint[2] memory _pk_y,
        uint[] memory _u_x,
        uint[] memory _u_y,
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

        if (_u_x.length != _u_y.length) {
            revert("To create points, we need as many x as y");
        }

        server = _server;
        public_key = Pairing.G2Point(_pk_x, _pk_y);

        for(uint i = 0; i < _u_x.length; i++){
            u.push(Pairing.G1Point(_u_x[i], _u_y[i]));
        }

        name = _name;
        file_size = _file_size;
    }

    /// @notice Anybody but the server calls this function in order to submit a
    ///         new challenge to server. Therefore, the server must answer this
    ///         challenge by a corresponding proof.
    ///         Adds the given challenge into the Challenge list.
    /// @param _i list that indicates the i-th bit of the original file to
    ///           challenge
    /// @param _nu list of factors chosen by the client making challenges
    ///            unpredictable
    function challenge(uint[] memory _i, uint[] memory _nu) public {
        if (msg.sender == server) {
            revert("The server is not able to call this function");
        }

        if (_i.length != _nu.length) {
            revert("Each i-th bit need a factor nu to compute the proof");
        }

        uint id = challenges.push(challenges.length);

        // i must NOT be superior to the number of signatures
        // number of signatures = file size / u.length
        for(uint j = 0; j < _i.length; j++) {
            index_challenges[id].i.push(_i[j] % (file_size/u.length));
            index_challenges[id].nu.push(_nu[j]);
        }
    }

    /// @notice Mathematical answer to the last challenge sent.
    ///         This function can only be called by the server.
    /// @param _mu First part of the proof (list)
    /// @param _sigma_x Abscissa of the second part of the proof
    /// @param _sigma_y Ordinate of the second part of the proof
    function proof(uint[] memory _mu, uint _sigma_x, uint _sigma_y) public {
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
        Pairing.G1Point memory h;
        Pairing.G1Point memory res_h;
        Pairing.G1Point memory res_u;
        Pairing.G1Point memory res;

        for (uint j = 0; j < c.i.length; j++) {
            h = Pairing.curveMul(Pairing.P1(), (name * c.i[j]));
            h = Pairing.curveMul(h, c.nu[j]);
            res_h = Pairing.addition(res_h, h);
        }

        for (uint j = 0; j < u.length; j++) {
            res_u = Pairing.curveMul(u[j], p.mu[j]);
            res = Pairing.addition(res, res_u);
        }

        res = Pairing.addition(res, res_h);

        p.valid = 
            Pairing.pairing2(
                Pairing.negate(p.sigma),
                Pairing.P2(),
                res,
                public_key
            );
    }

    /// @notice gets the challenge with the identifier given in parameter
    /// @param _id identifier of the challenge to return
    /// @return the challenge with the identifier <id>
    function getChallenge(uint _id) public view returns(Challenge memory) {
        return index_challenges[_id];
    }

    /// @notice gets the proof with the identifier given in parameter
    /// @param _id identifier of the proof to return
    /// @return the proof with identifier <id>
    function getProof(uint _id) public view returns(Proof memory) {
        return index_proofs[_id];
    }

    /// @notice gets the number of all challenges sent
    /// @return the number of all challenges sent
    function getChallengesLength() public view returns(uint) {
        return challenges.length;
    }

    /// @notice gets the number of all proofs sent
    /// @return the number of all proofs sent
    function getProofsLength() public view returns(uint) {
        return proofs.length;
    }
}