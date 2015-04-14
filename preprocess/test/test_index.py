#!/usr/bin/python

"""
@file
 This file is part of ASAGI.

 ASAGI is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of
 the License, or  (at your option) any later version.

 ASAGI is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.

 Diese Datei ist Teil von ASAGI.

 ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 der GNU Lesser General Public License, wie von der Free Software
 Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 modifizieren.

 ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.

 Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 mit diesem Programm erhalten haben. Wenn nicht, siehe
 <http://www.gnu.org/licenses/>.

@copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
"""
 
"""
Test suite for index calculations with blocks
"""
class TestIndex:
    def __init__(self, totalBlocks, nodes, cores, rr = True):
        self.__totalBlocks = totalBlocks
        self.__nodeCount = nodes
        self.__coreCount = cores
        self.__rr = rr
        
        self.__nodes = [-1] * totalBlocks
        self.__cores = [-1] * totalBlocks
        self.__offset = [-1] * totalBlocks
        self.__nodeOffset = [-1] * totalBlocks
        self.__local2global = [[[-1] * self._localBlockCount() for i in range(cores)] for i in range(nodes)]
        
        if rr:
            node = 0
            core = [0] * nodes
            offset = [0] * (nodes * cores)
            nodeOffset = [0] * (nodes * cores)
            for i in range(totalBlocks):
                self.__nodes[i] = node
                self.__cores[i] = core[node]
                self.__offset[i] = offset[node*cores + core[node]]
                self.__nodeOffset[i] = nodeOffset[node*cores + core[node]] + \
                    core[node] * self._localBlockCount()
                self.__local2global[node][core[node]][offset[node*cores + core[node]]] = i
                
                offset[node*cores + core[node]] += 1
                nodeOffset[node*cores + core[node]] += 1
                core[node] = (core[node] + 1) % cores                
                node = (node + 1) % nodes
        else:
            node = 0
            core = 0
            offset = 0
            nodeOffset = 0
            for i in range(totalBlocks):
                self.__nodes[i] = node
                self.__cores[i] = core
                self.__offset[i] = offset
                self.__nodeOffset[i] = nodeOffset
                self.__local2global[node][core][offset] = i
                
                offset += 1
                nodeOffset += 1
                if offset >= self._localBlockCount():
                    core += 1
                    offset = 0
                if core >= cores:
                    node += 1
                    core = 0
                    nodeOffset = 0
    
    def _nodeBlockCount(self):
        return self._localBlockCount() * self.__coreCount
    
    def _localBlockCount(self):
        cores = self.__coreCount * self.__nodeCount
        return (self.__totalBlocks + cores - 1) / cores
    
    def _blockRank(self, id):
        if self.__rr:
            return id % self.__nodeCount
        return id / self._nodeBlockCount()
    
    def _blockDomain(self, id):
        if self.__rr:
            return (id / self.__nodeCount) % self.__coreCount
        return (id / self._localBlockCount()) % self.__coreCount
        
    def _blockOffset(self, id):
        if self.__rr:
            return id / self.__nodeCount / self.__coreCount
        return id % self._localBlockCount()
    
    def _blockNodeOffset(self, id):
        return self._blockOffset(id) + self._blockDomain(id) * self._localBlockCount()
    
    def _local2global(self, id, core, node):
        if self.__rr:
            return (id * self.__coreCount + core) * self.__nodeCount + node
        return id + (core + node * self.__coreCount) * self._localBlockCount();
        
    def test(self):
        for i in range(self.__totalBlocks):
            assert self._blockRank(i) == self.__nodes[i], "%d %d %d %d" % (self.__rr, i, self._blockRank(i), self.__nodes[i])
            assert self._blockDomain(i) == self.__cores[i], "%d %d %d %d" % (self.__rr, i, self._blockDomain(i), self.__cores[i])
            assert self._blockOffset(i) == self.__offset[i], "%d %d %d %d" % (self.__rr, i, self._blockOffset(i), self.__offset[i])
            assert self._blockNodeOffset(i) == self.__nodeOffset[i], \
                "%d %d %d %d" % (self.__rr, i, self._blockNodeOffset(i), self.__nodeOffset[i])
        for n in range(self.__nodeCount):
            for c in range(self.__coreCount):
                for i in range(self._localBlockCount()):
                    if self._local2global(i, c, n) < self.__totalBlocks:
                        assert self._local2global(i, c, n) == self.__local2global[n][c][i], \
                            "%d %d %d %d %d %d" % (self.__rr, n, c, i, self._local2global(i, c, n), self.__local2global[n][c][i])
        
    
for t in [True, False]:
    TestIndex(20, 2, 2, t).test()
    TestIndex(50, 4, 2, t).test()
    TestIndex(32, 1, 4, t).test()
    TestIndex(32, 4, 1, t).test()
    TestIndex(7, 1, 1, t).test()