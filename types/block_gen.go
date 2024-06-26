package types

// Code generated by github.com/tinylib/msgp DO NOT EDIT.

import (
	"github.com/tinylib/msgp/msgp"
)

// DecodeMsg implements msgp.Decodable
func (z *Block) DecodeMsg(dc *msgp.Reader) (err error) {
	var field []byte
	_ = field
	var zb0001 uint32
	zb0001, err = dc.ReadMapHeader()
	if err != nil {
		err = msgp.WrapError(err)
		return
	}
	for zb0001 > 0 {
		zb0001--
		field, err = dc.ReadMapKeyPtr()
		if err != nil {
			err = msgp.WrapError(err)
			return
		}
		switch msgp.UnsafeString(field) {
		case "num":
			z.Number, err = dc.ReadInt64()
			if err != nil {
				err = msgp.WrapError(err, "Number")
				return
			}
		case "hash":
			err = dc.ReadExactBytes((z.Hash)[:])
			if err != nil {
				err = msgp.WrapError(err, "Hash")
				return
			}
		case "parent":
			err = dc.ReadExactBytes((z.ParentHash)[:])
			if err != nil {
				err = msgp.WrapError(err, "ParentHash")
				return
			}
		case "bloom":
			err = dc.ReadExactBytes((z.LogsBloom)[:])
			if err != nil {
				err = msgp.WrapError(err, "LogsBloom")
				return
			}
		case "troot":
			err = dc.ReadExactBytes((z.TransactionsRoot)[:])
			if err != nil {
				err = msgp.WrapError(err, "TransactionsRoot")
				return
			}
		case "sroot":
			err = dc.ReadExactBytes((z.StateRoot)[:])
			if err != nil {
				err = msgp.WrapError(err, "StateRoot")
				return
			}
		case "miner":
			err = dc.ReadExactBytes((z.Miner)[:])
			if err != nil {
				err = msgp.WrapError(err, "Miner")
				return
			}
		case "size":
			z.Size, err = dc.ReadInt64()
			if err != nil {
				err = msgp.WrapError(err, "Size")
				return
			}
		case "gasused":
			z.GasUsed, err = dc.ReadUint64()
			if err != nil {
				err = msgp.WrapError(err, "GasUsed")
				return
			}
		case "time":
			z.Timestamp, err = dc.ReadInt64()
			if err != nil {
				err = msgp.WrapError(err, "Timestamp")
				return
			}
		case "txs":
			var zb0002 uint32
			zb0002, err = dc.ReadArrayHeader()
			if err != nil {
				err = msgp.WrapError(err, "Transactions")
				return
			}
			if cap(z.Transactions) >= int(zb0002) {
				z.Transactions = (z.Transactions)[:zb0002]
			} else {
				z.Transactions = make([][32]byte, zb0002)
			}
			for za0007 := range z.Transactions {
				err = dc.ReadExactBytes((z.Transactions[za0007])[:])
				if err != nil {
					err = msgp.WrapError(err, "Transactions", za0007)
					return
				}
			}
		case "basefee":
			err = dc.ReadExactBytes((z.BaseFeePerGas)[:])
			if err != nil {
				err = msgp.WrapError(err, "BaseFeePerGas")
				return
			}
		default:
			err = dc.Skip()
			if err != nil {
				err = msgp.WrapError(err)
				return
			}
		}
	}
	return
}

// EncodeMsg implements msgp.Encodable
func (z *Block) EncodeMsg(en *msgp.Writer) (err error) {
	// map header, size 12
	// write "num"
	err = en.Append(0x8c, 0xa3, 0x6e, 0x75, 0x6d)
	if err != nil {
		return
	}
	err = en.WriteInt64(z.Number)
	if err != nil {
		err = msgp.WrapError(err, "Number")
		return
	}
	// write "hash"
	err = en.Append(0xa4, 0x68, 0x61, 0x73, 0x68)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.Hash)[:])
	if err != nil {
		err = msgp.WrapError(err, "Hash")
		return
	}
	// write "parent"
	err = en.Append(0xa6, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.ParentHash)[:])
	if err != nil {
		err = msgp.WrapError(err, "ParentHash")
		return
	}
	// write "bloom"
	err = en.Append(0xa5, 0x62, 0x6c, 0x6f, 0x6f, 0x6d)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.LogsBloom)[:])
	if err != nil {
		err = msgp.WrapError(err, "LogsBloom")
		return
	}
	// write "troot"
	err = en.Append(0xa5, 0x74, 0x72, 0x6f, 0x6f, 0x74)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.TransactionsRoot)[:])
	if err != nil {
		err = msgp.WrapError(err, "TransactionsRoot")
		return
	}
	// write "sroot"
	err = en.Append(0xa5, 0x73, 0x72, 0x6f, 0x6f, 0x74)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.StateRoot)[:])
	if err != nil {
		err = msgp.WrapError(err, "StateRoot")
		return
	}
	// write "miner"
	err = en.Append(0xa5, 0x6d, 0x69, 0x6e, 0x65, 0x72)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.Miner)[:])
	if err != nil {
		err = msgp.WrapError(err, "Miner")
		return
	}
	// write "size"
	err = en.Append(0xa4, 0x73, 0x69, 0x7a, 0x65)
	if err != nil {
		return
	}
	err = en.WriteInt64(z.Size)
	if err != nil {
		err = msgp.WrapError(err, "Size")
		return
	}
	// write "gasused"
	err = en.Append(0xa7, 0x67, 0x61, 0x73, 0x75, 0x73, 0x65, 0x64)
	if err != nil {
		return
	}
	err = en.WriteUint64(z.GasUsed)
	if err != nil {
		err = msgp.WrapError(err, "GasUsed")
		return
	}
	// write "time"
	err = en.Append(0xa4, 0x74, 0x69, 0x6d, 0x65)
	if err != nil {
		return
	}
	err = en.WriteInt64(z.Timestamp)
	if err != nil {
		err = msgp.WrapError(err, "Timestamp")
		return
	}
	// write "txs"
	err = en.Append(0xa3, 0x74, 0x78, 0x73)
	if err != nil {
		return
	}
	err = en.WriteArrayHeader(uint32(len(z.Transactions)))
	if err != nil {
		err = msgp.WrapError(err, "Transactions")
		return
	}
	for za0007 := range z.Transactions {
		err = en.WriteBytes((z.Transactions[za0007])[:])
		if err != nil {
			err = msgp.WrapError(err, "Transactions", za0007)
			return
		}
	}
	// write "basefee"
	err = en.Append(0xa7, 0x62, 0x61, 0x73, 0x65, 0x66, 0x65, 0x65)
	if err != nil {
		return
	}
	err = en.WriteBytes((z.BaseFeePerGas)[:])
	if err != nil {
		err = msgp.WrapError(err, "BaseFeePerGas")
		return
	}
	return
}

// MarshalMsg implements msgp.Marshaler
func (z *Block) MarshalMsg(b []byte) (o []byte, err error) {
	o = msgp.Require(b, z.Msgsize())
	// map header, size 12
	// string "num"
	o = append(o, 0x8c, 0xa3, 0x6e, 0x75, 0x6d)
	o = msgp.AppendInt64(o, z.Number)
	// string "hash"
	o = append(o, 0xa4, 0x68, 0x61, 0x73, 0x68)
	o = msgp.AppendBytes(o, (z.Hash)[:])
	// string "parent"
	o = append(o, 0xa6, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74)
	o = msgp.AppendBytes(o, (z.ParentHash)[:])
	// string "bloom"
	o = append(o, 0xa5, 0x62, 0x6c, 0x6f, 0x6f, 0x6d)
	o = msgp.AppendBytes(o, (z.LogsBloom)[:])
	// string "troot"
	o = append(o, 0xa5, 0x74, 0x72, 0x6f, 0x6f, 0x74)
	o = msgp.AppendBytes(o, (z.TransactionsRoot)[:])
	// string "sroot"
	o = append(o, 0xa5, 0x73, 0x72, 0x6f, 0x6f, 0x74)
	o = msgp.AppendBytes(o, (z.StateRoot)[:])
	// string "miner"
	o = append(o, 0xa5, 0x6d, 0x69, 0x6e, 0x65, 0x72)
	o = msgp.AppendBytes(o, (z.Miner)[:])
	// string "size"
	o = append(o, 0xa4, 0x73, 0x69, 0x7a, 0x65)
	o = msgp.AppendInt64(o, z.Size)
	// string "gasused"
	o = append(o, 0xa7, 0x67, 0x61, 0x73, 0x75, 0x73, 0x65, 0x64)
	o = msgp.AppendUint64(o, z.GasUsed)
	// string "time"
	o = append(o, 0xa4, 0x74, 0x69, 0x6d, 0x65)
	o = msgp.AppendInt64(o, z.Timestamp)
	// string "txs"
	o = append(o, 0xa3, 0x74, 0x78, 0x73)
	o = msgp.AppendArrayHeader(o, uint32(len(z.Transactions)))
	for za0007 := range z.Transactions {
		o = msgp.AppendBytes(o, (z.Transactions[za0007])[:])
	}
	// string "basefee"
	o = append(o, 0xa7, 0x62, 0x61, 0x73, 0x65, 0x66, 0x65, 0x65)
	o = msgp.AppendBytes(o, (z.BaseFeePerGas)[:])
	return
}

// UnmarshalMsg implements msgp.Unmarshaler
func (z *Block) UnmarshalMsg(bts []byte) (o []byte, err error) {
	var field []byte
	_ = field
	var zb0001 uint32
	zb0001, bts, err = msgp.ReadMapHeaderBytes(bts)
	if err != nil {
		err = msgp.WrapError(err)
		return
	}
	for zb0001 > 0 {
		zb0001--
		field, bts, err = msgp.ReadMapKeyZC(bts)
		if err != nil {
			err = msgp.WrapError(err)
			return
		}
		switch msgp.UnsafeString(field) {
		case "num":
			z.Number, bts, err = msgp.ReadInt64Bytes(bts)
			if err != nil {
				err = msgp.WrapError(err, "Number")
				return
			}
		case "hash":
			bts, err = msgp.ReadExactBytes(bts, (z.Hash)[:])
			if err != nil {
				err = msgp.WrapError(err, "Hash")
				return
			}
		case "parent":
			bts, err = msgp.ReadExactBytes(bts, (z.ParentHash)[:])
			if err != nil {
				err = msgp.WrapError(err, "ParentHash")
				return
			}
		case "bloom":
			bts, err = msgp.ReadExactBytes(bts, (z.LogsBloom)[:])
			if err != nil {
				err = msgp.WrapError(err, "LogsBloom")
				return
			}
		case "troot":
			bts, err = msgp.ReadExactBytes(bts, (z.TransactionsRoot)[:])
			if err != nil {
				err = msgp.WrapError(err, "TransactionsRoot")
				return
			}
		case "sroot":
			bts, err = msgp.ReadExactBytes(bts, (z.StateRoot)[:])
			if err != nil {
				err = msgp.WrapError(err, "StateRoot")
				return
			}
		case "miner":
			bts, err = msgp.ReadExactBytes(bts, (z.Miner)[:])
			if err != nil {
				err = msgp.WrapError(err, "Miner")
				return
			}
		case "size":
			z.Size, bts, err = msgp.ReadInt64Bytes(bts)
			if err != nil {
				err = msgp.WrapError(err, "Size")
				return
			}
		case "gasused":
			z.GasUsed, bts, err = msgp.ReadUint64Bytes(bts)
			if err != nil {
				err = msgp.WrapError(err, "GasUsed")
				return
			}
		case "time":
			z.Timestamp, bts, err = msgp.ReadInt64Bytes(bts)
			if err != nil {
				err = msgp.WrapError(err, "Timestamp")
				return
			}
		case "txs":
			var zb0002 uint32
			zb0002, bts, err = msgp.ReadArrayHeaderBytes(bts)
			if err != nil {
				err = msgp.WrapError(err, "Transactions")
				return
			}
			if cap(z.Transactions) >= int(zb0002) {
				z.Transactions = (z.Transactions)[:zb0002]
			} else {
				z.Transactions = make([][32]byte, zb0002)
			}
			for za0007 := range z.Transactions {
				bts, err = msgp.ReadExactBytes(bts, (z.Transactions[za0007])[:])
				if err != nil {
					err = msgp.WrapError(err, "Transactions", za0007)
					return
				}
			}
		case "basefee":
			bts, err = msgp.ReadExactBytes(bts, (z.BaseFeePerGas)[:])
			if err != nil {
				err = msgp.WrapError(err, "BaseFeePerGas")
				return
			}
		default:
			bts, err = msgp.Skip(bts)
			if err != nil {
				err = msgp.WrapError(err)
				return
			}
		}
	}
	o = bts
	return
}

// Msgsize returns an upper bound estimate of the number of bytes occupied by the serialized message
func (z *Block) Msgsize() (s int) {
	s = 1 + 4 + msgp.Int64Size + 5 + msgp.ArrayHeaderSize + (32 * (msgp.ByteSize)) + 7 + msgp.ArrayHeaderSize + (32 * (msgp.ByteSize)) + 6 + msgp.ArrayHeaderSize + (256 * (msgp.ByteSize)) + 6 + msgp.ArrayHeaderSize + (32 * (msgp.ByteSize)) + 6 + msgp.ArrayHeaderSize + (32 * (msgp.ByteSize)) + 6 + msgp.ArrayHeaderSize + (20 * (msgp.ByteSize)) + 5 + msgp.Int64Size + 8 + msgp.Uint64Size + 5 + msgp.Int64Size + 4 + msgp.ArrayHeaderSize + (len(z.Transactions) * (32 * (msgp.ByteSize))) + 8 + msgp.ArrayHeaderSize + (32 * (msgp.ByteSize))
	return
}
