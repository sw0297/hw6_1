
// JSON_Post_redacted.cpp
// #define _ECS36B_DEBUG_

#include <arpa/inet.h>
#include "Post.h"

std::map<std::string, OKey *> All_Keys;

std::vector<Post *> *
getPostbyKey
(std::string key)
{
  if (All_Keys.find(key) == All_Keys.end())
    return NULL;

  OKey *okey_ptr = All_Keys[key];
  return okey_ptr->posts;
}

OKey *
getOKey
(std::string key)
{
  if (All_Keys.find(key) != All_Keys.end())
    return All_Keys[key];

  OKey *new_okey_ptr = new OKey(key);
  new_okey_ptr->posts = new std::vector<Post *>();
  All_Keys[key] = new_okey_ptr;
  return new_okey_ptr;
}

std::string
Json_2_PostID
(Json::Value myv)
{
  return ((myv["id"]).asString());
}

// here it is

Post *
Json_2_Post
(Json::Value myv)
{ //std::cout << myv << std::endl;
  // doesn't print all the comments and all reactions, get a segementation fault 11
  if (myv.isArray() == true)
    {
      // wrong one
      // {...}
      // [...] NULL
      return NULL;
    }

  Post * my_post_ptr = new Post((Person *) NULL, (Message *) NULL);

  // "id"
  if ((myv["id"]).isNull() != true)
    {
      char idstr[1024];
      bzero(idstr, 1024);
      sprintf(idstr, "%s", ((myv["id"]).asString()).c_str());
      if (idstr[0] != '\0')
	{
	  char c_prof_buf[256];
	  char c_post_buf[256];
	  bzero(c_prof_buf, 256);
	  bzero(c_post_buf, 256);

	  sscanf(idstr, "%[^_]_%s", c_prof_buf, c_post_buf);

	  std::string profile_id = { c_prof_buf };
	  std::string post_id = { c_post_buf };

	  // put id attributes to the Post object
	  my_post_ptr->profile_id = profile_id;
	  my_post_ptr->post_id = post_id;
	}
      else
	{//std::cerr << "First error" << std::endl;
	  std::cerr << "No Post ID presented" << std::endl;
	  exit(-1);
	}
    }
  else
    {// error occurs here when running search clause b/c id is null
      std::cerr << "No Post ID presented" << std::endl;
      exit(-2);
    }

  int rc = Json_Post_Common(myv, my_post_ptr);
  if (rc != 0)
    {
      std::cerr << "Error Json_Post_Common " << rc << std::endl;
      exit(-3);
    }

  return my_post_ptr;
}

int
Json_Post_Merging
(Json::Value myv, Post * my_post_ptr)
{
  char idstr[1024];
  bzero(idstr, 1024);
  
  if ((myv).isArray() == true) return -3;
  if ((myv["id"]).isNull() == true) return -1;

  // compare ID first
  // "id" (Let us check first if they share the same ID)
  sprintf(idstr, "%s", ((myv["id"]).asString()).c_str());

  if (idstr[0] != '\0')
    {
      char c_prof_buf[256];
      char c_post_buf[256];
      bzero(c_prof_buf, 256);
      bzero(c_post_buf, 256);

      sscanf(idstr, "%[^_]_%s", c_prof_buf, c_post_buf);

      std::string profile_id = { c_prof_buf };
      std::string post_id = { c_post_buf };

      if ((my_post_ptr->profile_id != profile_id) ||
	  (my_post_ptr->post_id    != post_id))
	{	  
	  std::cerr << "Post IDs mismatched!!" << std::endl;
	  return -1;
	}
    }
  else
    {
      std::cerr << "No Post ID presented for the second JSON" << std::endl;
      return -2;
    }

  // now, they matched, let us proceed with the merge action
  return Json_Post_Common(myv, my_post_ptr);
}

int
Json_Post_Common
(Json::Value myv, Post * my_post_ptr)
{
  int i, j;

  // under Json_Post_Common, we assume that the ID already matched
  // for both Post and myv

  // "actions"
//...
  if (((myv["actions"]).isNull() != true) && 
      ((myv["actions"]["count"]).asInt() > 0))
{
  if (my_post_ptr->actions == NULL)
    {
      my_post_ptr->actions = new vector<Action *>();
    }
  for (i = 0; i < (myv["actions"]["count"]).asInt(); i++)
    {
      Json::Value l_jv = myv["actions"]["data"][i];
      //std::cout << l_jv << std::endl; 
      //prints the 2 posts correctly, still displays clinet error message
      if (((l_jv["class_id"]).isNull() != true) &&
	  ((l_jv["host_url"]).isNull() != true) &&
	  ((l_jv["name"]).isNull() != true) &&
	  ((l_jv["object_id"]).isNull() != true) &&
	  ((l_jv["owner_vsID"]).isNull() != true))
	{
	  Action * act_ptr = new Action((l_jv["class_id"]).asString(),
					(l_jv["host_url"]).asString(),
					(l_jv["name"]).asString(),
					(l_jv["object_id"]).asString(),
					(l_jv["owner_vsID"]).asString());
	  //std::cout << &(act_ptr) << std::endl;
	  //posts are stored in a valid address
	  (*(my_post_ptr->actions)).push_back(act_ptr);
	}
    }
}
  // "comments"
//...
  if (((myv["comments"]).isNull() != true) &&
      ((myv["comments"]["count"]).asInt() > 0))
    { //std::cout << (myv["comments"]) << std::endl;
      // only showing 2 comments instead of 3
      if (my_post_ptr->comments == NULL)
	{
	  my_post_ptr->comments = new vector<Comment *>();
	}
      for (i = 0; i < (myv["comments"]["count"]).asInt(); i++)
	{ //std::cout << (myv["comments"]["count"]).asInt() << std::endl; --> 2 instead of 3
	  Json::Value l_jv = myv["comments"]["data"][i];
	  //std::cout << l_jv << std::endl; --> get the first 2 posts, missing last one
	  // also get the error (even in action): 
	  
	  Comment * comment_ptr;
	  char buf_profile[1024];
          bzero(buf_profile, 1024);
          char buf_post[1024];
          bzero(buf_post, 1024);
          char buf_comment[1024];
          bzero(buf_comment, 1024); 
          sscanf((l_jv["id"].asString()).c_str(), "%[^_]_%[^_]_%s",
	  	 buf_profile, buf_post, buf_comment);
        std::string s_profile {buf_profile};
        std::string s_post {buf_post};
        std::string s_comment {buf_comment};
	  
	  if //(((s_profile).isNull() != true) &&
	    //((s_post).isNull() != true) &&
	    //((s_comment).isNull() != true) &&
	    (((l_jv["id"]).isNull() != true) &&
	      ((l_jv["from"]["avatar_name"]).isNull() != true) &&
	      ((l_jv["from"]["vsID"]).isNull() != true) &&
	      ((l_jv["message"]).isNull() != true) &&
	     ((l_jv["created_time"]).isNull() != true))
	    {
	      // Person
	      Person * person_ptr = new Person((l_jv["from"]["avatar_name"]).asString(),
					       (l_jv["from"]["vsID"]).asString());
	      Message * message_ptr = new Message((l_jv["message"]).asString());
        
	      JvTime * create_time_ptr = new JvTime(((l_jv["created_time"]).asString()).c_str());
        
	      comment_ptr = new Comment(s_profile, s_post, s_comment,
						  person_ptr, message_ptr, create_time_ptr);
	     }
	  // reactions part
	  vector<Reaction *> * vec_c_reactions_ptr = new vector<Reaction *>();
	  for (j = 0; j < (l_jv["reactions"]["count"]).asInt(); j++)
	    {
	      Json::Value o_jv = l_jv["reactions"]["data"][j];
	      
	      if (((o_jv["avatar_name"]).isNull() != true) &&
		  ((o_jv["vsID"]).isNull() != true) &&
		  ((o_jv["created_time"]).isNull() != true) &&
		  ((o_jv["type"]).isNull() != true))
		{
		  Person * person2_ptr = new Person((o_jv["avatar_name"]).asString(),
                                                    (o_jv["vsID"]).asString());
		  JvTime * create_time2_ptr = new JvTime(((o_jv["created_time"]).asString()).c_str());
		  Reaction * reaction_cr_ptr = new Reaction((o_jv["type"]).asString(), 
							    person2_ptr, create_time2_ptr);
		  (*vec_c_reactions_ptr).push_back(reaction_cr_ptr);
		}
	    }
	  comment_ptr->reactions = vec_c_reactions_ptr;

	  // tags part 
	  vector<Tag      *> * vec_tag_ptr = new vector<Tag *>();
	  for (j = 0; j < (l_jv["tags"]["count"]).asInt(); j++)
	    { 
	      Json::Value t_jv = l_jv["tags"]["data"][j];
	      if (((t_jv["index"]).isNull() != true) &&
		  ((t_jv["avatar_name"]).isNull() != true) &&
		  ((t_jv["vsID"]).isNull() != true) &&
		  ((t_jv["comment"]).isNull() != true))
		{
		  Tag * tag_ptr = new Tag((t_jv["index"]).asInt(),
					  (t_jv["avatar_name"]).asString(),
					  (t_jv["vsID"]).asString(),
					  (t_jv["comment"]).asString());
		  (*vec_tag_ptr).push_back(tag_ptr);
		}
	    }
	  comment_ptr->tags = vec_tag_ptr;
	  (*(my_post_ptr->comments)).push_back(comment_ptr);
	}
    }

  // "keys"
  //...
  if (((myv["keys"]).isNull() != true) &&
      ((myv["keys"]["count"]).asInt() > 0))
    {
      if (my_post_ptr->keys == NULL)
	{
	  my_post_ptr->keys = new vector<OKey *>();
	}
      for (i = 0; i < (myv["keys"]["count"]).asInt(); i++) 
	{
	  Json::Value l_jv = myv["keys"]["data"][i];
	  //std::cout << l_jv << std::endl;
	  // gets 3 keys as expected
	  if ((l_jv.isNull() != true))
	    {
	      OKey * okey_ptr = getOKey(l_jv.asString());
	      // adding the post the keyword object
	      (*(okey_ptr->posts)).push_back(my_post_ptr); 
	      (*(my_post_ptr->keys)).push_back(okey_ptr);
	      
	    }
	}
    }

  // "links"
//...
if (((myv["links"]).isNull() != true) &&
    ((myv["links"]["count"]).asInt() > 0))
  {
    if (my_post_ptr->links == NULL)
      {
	my_post_ptr->links = new vector<Link *>();
      }
    for (i = 0; i < (myv["links"]["count"]).asInt(); i++)
      {
	Json::Value l_jv = myv["links"]["data"][i];
	if (((l_jv["url"]).isNull() != true) &&
	    ((l_jv["class_id"]).isNull() != true) &&
	    ((l_jv["object_id"]).isNull() != true))
	  {
	    Link * link_ptr = new Link((l_jv["url"]).asString(),
				       (l_jv["class_ud"]).asString(),
				       (l_jv["object_id"]).asString());
	    (*(my_post_ptr->links)).push_back(link_ptr);
	  }
      }
  }

  // "message"
//...
if ((myv["message"]).isNull() != true)
  {
    Message * msg_ptr = new Message(myv["message"].asString());
    my_post_ptr->msg = msg_ptr; 
  }

  // "reactions" 
    if (((myv["reactions"]).isNull() != true) &&
	((myv["reactions"]["count"]).asInt() > 0))
    {
      if (my_post_ptr->reactions == NULL)
	my_post_ptr->reactions = new vector<Reaction *>();

      for (i = 0; i < (myv["reactions"]["count"]).asInt(); i++)
	{
	  Json::Value l_jv = myv["reactions"]["data"][i];

	  if (((l_jv["avatar_name"]).isNull() != true) &&
	      ((l_jv["vsID"]).isNull() != true) &&
	      ((l_jv["created_time"]).isNull() != true) &&
	      ((l_jv["type"]).isNull() != true))
	    {

	      Person *react_p_ptr = new Person((l_jv["avatar_name"]).asString(),
					       (l_jv["vsID"]).asString()); 
	      JvTime *react_cr_time_ptr =
		new JvTime(((l_jv["created_time"]).asString()).c_str());
	      Reaction * react_ptr = new Reaction(l_jv["type"].asString(),
						  react_p_ptr, react_cr_time_ptr);

	      (*(my_post_ptr->reactions)).push_back(react_ptr);

	    }
	}
    }

  // "history" --> for hw7
    //...

  // "id"
  // already done

  // "from"
  // this could be a from replacement
    //...
if (((myv["from"]).isNull() != true))
  {
    Person * author_ptr = new Person((myv["from"]["avatar_name"]).asString(),
				     (myv["from"]["vsID"]).asString());
    my_post_ptr->author = author_ptr;
  }
  // "to"
    if (((myv["to"]).isNull() != true) &&
	((myv["to"]["count"]).asInt() > 0))
    {
      if (my_post_ptr->receivers == NULL)
	my_post_ptr->receivers = new vector<Person *>();

      for (i = 0; i < (myv["to"]["count"]).asInt(); i++)
	{
	  Json::Value l_jv = myv["to"]["data"][i];

	  if (((l_jv["avatar_name"]).isNull() != true) &&
	      ((l_jv["vsID"]).isNull() != true))
	    {
	      Person * to_ptr = new Person((l_jv["avatar_name"]).asString(),
					   (l_jv["vsID"]).asString());

	      (*(my_post_ptr->receivers)).push_back(to_ptr);
	    }
	}
    }

  // "created_time"
  if ((myv["created_time"]).isNull() != true)
    {
      my_post_ptr->created = new JvTime(((myv["created_time"]).asString()).c_str());
    }

  // "updated_time"
//...
  if ((myv["updated_time"]).isNull() != true)
  {
    my_post_ptr->updated = new JvTime(((myv["updated_time"]).asString()).c_str());
  }

  // "is_published"
  my_post_ptr->is_published = (myv["is_published"]).asBool();

  return 0;
}
