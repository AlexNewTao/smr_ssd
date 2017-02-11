


void initSSDTable(size_t size)
{
	ssd_hashtable = (SSDHashBucket *)malloc(sizeof(SSDHashBucket)*size);
	size_t i;
	SSDHashBucket *ssd_hash = ssd_hashtable;
	for (i = 0; i < size; ssd_hash++, i++){
		ssd_hash->ssd_id = -1;
		ssd_hash->hash_key.offset = -1;
		ssd_hash->next_item = NULL;
	}
}

//得到ssdtable的hashcode
unsigned long ssdtableHashcode(SSDTag *ssd_tag)
{
	unsigned long ssd_hash = (ssd_tag->offset / SSD_BUFFER_SIZE) % NSSDTables;
	return ssd_hash;
}


long ssdtableLookup(SSDTag *ssd_tag, unsigned long hash_code)
{
	if (DEBUG)
		printf("[INFO] Lookup ssd_tag: %lu\n",ssd_tag->offset);

	//得到hash的bucket，大小为ssd_hashtable + (unsigned long) (hash_code)
	SSDHashBucket *nowbucket = GetSSDHashBucket(hash_code);

	//循环的条件是不为空
	while (nowbucket != NULL) {
	//	printf("nowbucket->buf_id = %u %u %u\n", nowbucket->hash_key.rel.database, nowbucket->hash_key.rel.relation, nowbucket->hash_key.block_num);
		//如果找到hash key，返回，否则接着往后寻找
		if (isSamessd(&nowbucket->hash_key, ssd_tag)) {
	//		printf("find\n");
			return nowbucket->ssd_id;
		}
		nowbucket = nowbucket->next_item;
	}
//	printf("no find\n");

	//如果没有找到就返回-1；
	return -1;
}





long ssdtableDelete(SSDTag *ssd_tag, unsigned long hash_code)
{
	if (DEBUG)
		printf("[INFO] Delete ssd_tag: %lu, hash_code=%lu\n",ssd_tag->offset, hash_code);
	SSDHashBucket *nowbucket = GetSSDHashBucket(hash_code);
	long del_id;
	SSDHashBucket *delitem;
	nowbucket->next_item;
	while (nowbucket->next_item != NULL && nowbucket != NULL) {
		if (isSamessd(&nowbucket->next_item->hash_key, ssd_tag)) {
			del_id = nowbucket->next_item->ssd_id;
			break;
		}
		nowbucket = nowbucket->next_item;
	}
	//printf("not found2\n");
	if (isSamessd(&nowbucket->hash_key, ssd_tag)) {
		del_id = nowbucket->ssd_id;
	}
	//printf("not found3\n");
	if (nowbucket->next_item != NULL) {
		delitem = nowbucket->next_item;
		nowbucket->next_item = nowbucket->next_item->next_item;
		free(delitem);
		return del_id;
	}
	else {
		delitem = nowbucket->next_item;
		nowbucket->next_item = NULL;
		free(delitem);
		return del_id;
	}

	return -1;
}








static bool isSamessd(SSDTag *tag1, SSDTag *tag2)
{
	if (tag1->offset != tag2->offset)
		return 0;
	else return 1;
}



